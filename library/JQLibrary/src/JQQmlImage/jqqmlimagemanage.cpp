// .h include
#include "jqqmlimagemanage.h"

// Qt lib import
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickImageProvider>
#include <QQuickTextureFactory>
#include <QFile>
#include <QStandardPaths>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QTime>
#include <QMap>
#include <QtConcurrent>

// DesayTextureFactory
struct alignas( 8 ) ImageInformationHead
{
    qint32 imageWidth;
    qint32 imageHeight;
    qint32 imageFormat;
    qint32 imageFileSize;
    qint64 imageLastModified;
};

class DesayTextureFactory: public QQuickTextureFactory
{
public:
    DesayTextureFactory(const QString &id):
        id_( id )
    {
        if ( id.isEmpty() ) { return; }

        const auto &&localFilePath = QUrl( id ).toLocalFile();
        if ( localFilePath.isEmpty() )
        {
            qDebug() << "DesayTextureFactory::DesayTextureFactory: id error:" << id;
            return;
        }

        const auto &&jqicFilePath = JQQmlImageManage::jqicFilePath( QUrl( id ).toLocalFile() );
        QFile jqicFile( jqicFilePath );

        ImageInformationHead imageInformationHead;

        if ( cacheData_.contains( jqicFilePath ) )
        {
            // 有匹配的预加载数据，直接使用预加载数据
//            qDebug() << "mode 1:" << jqicFilePath;

            const auto &dataPair = cacheData_[ jqicFilePath ];

            memcpy( &imageInformationHead, dataPair.first.constData(), sizeof( ImageInformationHead ) );

            buffer_ = dataPair.second;
            image_ = QImage(
                        ( const uchar * )buffer_.constData(),
                        imageInformationHead.imageWidth,
                        imageInformationHead.imageHeight,
                        ( QImage::Format )imageInformationHead.imageFormat
                    );
        }
        else if ( jqicFile.exists() && ( jqicFile.size() >= ( qint64 )sizeof( ImageInformationHead ) ) )
        {
            // 在本地发现缓存，直接加载缓存数据
//            qDebug() << "mode 2:" << jqicFilePath;

            jqicFile.open( QIODevice::ReadOnly );

            const auto &&headData = jqicFile.read( sizeof( ImageInformationHead ) );
            memcpy( &imageInformationHead, headData.constData(), sizeof( ImageInformationHead ) );

            buffer_ = jqicFile.readAll();
            image_ = QImage(
                        ( const uchar * )buffer_.constData(),
                        imageInformationHead.imageWidth,
                        imageInformationHead.imageHeight,
                        ( QImage::Format )imageInformationHead.imageFormat
                    );
        }
        else
        {
            // 在本地没有发现图片缓存，那么重新加载图片
//            qDebug() << "mode 3:" << jqicFilePath;

            QTime timeForLoad;

            timeForLoad.start();
            image_.load( localFilePath );
            const auto &&loadElapsed = timeForLoad.elapsed();

            if ( image_.isNull() )
            {
                qDebug() << "DesayTextureFactory::DesayTextureFactory: load error:" << localFilePath;
                return;
            }

            // 加载很快的图片（小于5ms）不进行缓存
            if ( loadElapsed < 5 ) { return; }

            const auto &&jqicFileInfo = QFileInfo( jqicFilePath );
            const auto &&jqicPath = jqicFileInfo.path();
            if ( !QFileInfo( jqicPath ).exists() )
            {
                QDir().mkpath( jqicPath );
            }

            imageInformationHead.imageWidth = image_.width();
            imageInformationHead.imageHeight = image_.height();
            imageInformationHead.imageFormat = image_.format();
            imageInformationHead.imageFileSize = jqicFileInfo.size();
            imageInformationHead.imageLastModified = jqicFileInfo.lastModified().toMSecsSinceEpoch();

            // 到新线程去存储，不影响主线程
            QtConcurrent::run(
                        [
                            jqicFilePath,
                            headData = QByteArray( (const char *)&imageInformationHead, sizeof( ImageInformationHead ) ),
                            imageData = QByteArray( (const char *)image_.constBits(), image_.byteCount() )
                        ]()
            {
                QFile jqicFile( jqicFilePath );

                jqicFile.open( QIODevice::WriteOnly );
                jqicFile.resize( headData.size() + imageData.size() );

                jqicFile.write( headData );
                jqicFile.write( imageData );

                jqicFile.waitForBytesWritten( 30 * 1000 );
            } );
        }
    }

    ~DesayTextureFactory()
    { }

    QSGTexture *createTexture(QQuickWindow *window) const
    {
        return window->createTextureFromImage( image_ );
    }

    int textureByteCount() const
    {
        return image_.byteCount();
    }

    QSize textureSize() const
    {
        return image_.size();
    }

    static bool preload(const QString &jqicFilePath)
    {
        QFile jqicFile( jqicFilePath );

        if ( !jqicFile.exists() || ( jqicFile.size() < ( qint64 )sizeof( ImageInformationHead ) ) )
        {
            return false;
        }

        jqicFile.open( QIODevice::ReadOnly );

        auto &dataPair = cacheData_[ jqicFilePath ];

        dataPair.first = jqicFile.read( sizeof( ImageInformationHead ) );
        dataPair.second = jqicFile.readAll();

        return true;
    }

private:
    QString id_;
    QImage image_;
    QByteArray buffer_;

    static QMap< QString, QPair< QByteArray, QByteArray > > cacheData_; // jqicFilePath -> { headData, imageData }
};

QMap< QString, QPair< QByteArray, QByteArray > > DesayTextureFactory::cacheData_;

// DesayImageProvider
class DesayImageProvider: public QQuickImageProvider
{
public:
    DesayImageProvider():
        QQuickImageProvider( QQmlImageProviderBase::Texture )
    { }

    ~DesayImageProvider() = default;

    QQuickTextureFactory *requestTexture(const QString &id, QSize *, const QSize &)
    {
        return new DesayTextureFactory( id );
    }
};

// JQQmlImageManage
QPointer< QQmlApplicationEngine > JQQmlImageManage::qmlApplicationEngine_;

JQQmlImageManage::JQQmlImageManage()
{
    qmlApplicationEngine_->addImageProvider( "JQQmlImage", new DesayImageProvider );
}

bool JQQmlImageManage::preload(const QString &imageFilePath)
{
    return DesayTextureFactory::preload( jqicFilePath( imageFilePath ) );
}

QString JQQmlImageManage::jqicPath()
{
    return QString( "%1/jqqmlimagecache" ).
            arg( QStandardPaths::writableLocation( QStandardPaths::CacheLocation ) );
}

QString JQQmlImageManage::jqicFilePath(const QString &imageFilePath)
{
    const auto &&imageFileInfo = QFileInfo( imageFilePath );
    if ( !imageFileInfo.isFile() ) { return { }; }

    QByteArray sumString;

    sumString += imageFilePath;
    sumString += QByteArray::number( imageFileInfo.lastModified().toMSecsSinceEpoch() );

    const auto &&md5String = QCryptographicHash::hash( sumString, QCryptographicHash::Md5 ).toHex();

    return QString( "%1/jqqmlimagecache/%2.jqic" ).
            arg( QStandardPaths::writableLocation( QStandardPaths::CacheLocation ), md5String.constData() );
}
