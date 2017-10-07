// .h include
#include "jqqmlimagemanage.h"

// C++ lib import
#include <algorithm>

// Qt lib import
#include <QDebug>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQuickWindow>
#include <QQuickImageProvider>
#include <QQuickTextureFactory>
#include <QFile>
#include <QStandardPaths>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QTime>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>
#include <QtConcurrent>

struct PreloadCacheData
{
    QSharedPointer< QMutex > mutexForPreload_; // 预读取的时候会lock，读取完毕立即unlock和释放这个mutex
    QByteArray headData;
    QByteArray imageData;
};

// JQQmlImageTextureFactory
class JQQmlImageTextureFactory: public QQuickTextureFactory
{
public:
    JQQmlImageTextureFactory(const QString &id):
        id_( id )
    {
        if ( id.isEmpty() ) { return; }

        QString imageFilePath;

        // 对ID进行预处理，得到图片的file path
        if ( !id.startsWith( ":/" ) )
        {
            if ( id.startsWith( "qrc:/" ) )
            {
                imageFilePath = ":/";
                imageFilePath += id.mid( 5 );
            }
            else if ( id.startsWith( "file:/" ) )
            {
                imageFilePath = QUrl( id ).toLocalFile();
            }
            else
            {
                qDebug() << "JQQmlImageTextureFactory: id error(1):" << id;
                return;
            }
        }

        if ( imageFilePath.isEmpty() )
        {
            qDebug() << "JQQmlImageTextureFactory: id error(2):" << id;
            return;
        }

        const auto &&jqicFilePath = JQQmlImageManage::jqicFilePath( imageFilePath );
        QFile jqicFile( jqicFilePath );

        JQQmlImageInformationHead imageInformationHead;

        if ( preloadCacheDatas_.contains( jqicFilePath ) )
        {
            // 有匹配的预加载数据，直接使用预加载数据

            const auto &preloadCacheData = preloadCacheDatas_[ jqicFilePath ];
            auto mutex = preloadCacheData.mutexForPreload_;
            if ( mutex )
            {
                // 如果已经进入了预加载列表但是实际上没有读取完，那么进行等待

                mutex->lock();
                mutex->unlock();
            }

            memcpy( &imageInformationHead, preloadCacheData.headData.constData(), sizeof( JQQmlImageInformationHead ) );

            buffer_ = preloadCacheData.imageData;
            image_ = QImage(
                        reinterpret_cast< const uchar * >( buffer_.constData() ),
                        imageInformationHead.imageWidth,
                        imageInformationHead.imageHeight,
                        static_cast< QImage::Format >( imageInformationHead.imageFormat )
                    );

            JQQmlImageManage::jqQmlImageManage()->recordImageFilePath( imageFilePath );
        }
        else if ( jqicFile.exists() &&
                ( jqicFile.size() >= static_cast< qint64 >( sizeof( JQQmlImageInformationHead ) ) ) )
        {
            // 在本地发现缓存，直接加载缓存数据

            if ( !jqicFile.open( QIODevice::ReadOnly ) )
            {
                qDebug() << "JQQmlImageTextureFactory: open file error:" << jqicFilePath;
                return;
            }

            jqicFile.read( reinterpret_cast< char * >( &imageInformationHead ), sizeof( JQQmlImageInformationHead ) );

            buffer_ = jqicFile.readAll();
            image_ = QImage(
                        reinterpret_cast< const uchar * >( buffer_.constData() ),
                        imageInformationHead.imageWidth,
                        imageInformationHead.imageHeight,
                        static_cast< QImage::Format >( imageInformationHead.imageFormat )
                    );
            image_.setColorCount( imageInformationHead.imageColorCount );

            JQQmlImageManage::jqQmlImageManage()->recordImageFilePath( imageFilePath );
        }
        else
        {
            // 在本地没有发现图片缓存，那么重新加载图片

            QTime timeForLoad;

            timeForLoad.start();
            image_.load( imageFilePath );
            const auto &&loadElapsed = timeForLoad.elapsed();

            if ( image_.isNull() )
            {
                qDebug() << "JQQmlImageTextureFactory: load error:" << imageFilePath;
                return;
            }

            // 加载很快的图片不进行缓存
            if ( loadElapsed < 3 ) { return; }

            // 内容过少的图片不进行缓存
            if ( image_.byteCount() <= ( 30 * 30 * 4 ) ) { return; }

            if ( ( image_.format() == QImage::Format_Mono ) ||
                 ( image_.format() == QImage::Format_Indexed8 ))
            {
                if ( image_.hasAlphaChannel() )
                {
                    image_ = image_.convertToFormat( QImage::Format_ARGB32 );
                }
                else
                {
                    image_ = image_.convertToFormat( QImage::Format_RGB888 );
                }
            }

            imageInformationHead.imageWidth = image_.width();
            imageInformationHead.imageHeight = image_.height();
            imageInformationHead.imageFormat = image_.format();
            imageInformationHead.imageColorCount = image_.colorCount();

            const auto &&headData = QByteArray( reinterpret_cast< const char * >( &imageInformationHead ), sizeof( JQQmlImageInformationHead ) );
            const auto &&imageData = QByteArray( reinterpret_cast< const char * >( image_.constBits() ), image_.byteCount() );

            // 到新线程去存储缓存文件，不影响主线程
            QtConcurrent::run(
                        [
                            jqicFilePath,
                            headData,
                            imageData
                        ]()
            {
                QFile jqicFile( jqicFilePath );

                if ( !jqicFile.open( QIODevice::WriteOnly ) )
                {
                    qDebug() << "JQQmlImageTextureFactory: open file error:" << jqicFilePath;
                    return;
                }
                jqicFile.resize( headData.size() + imageData.size() );

                jqicFile.write( headData );
                jqicFile.write( imageData );

                jqicFile.waitForBytesWritten( 30 * 1000 );
            } );

            JQQmlImageManage::jqQmlImageManage()->recordImageFilePath( imageFilePath );
        }
    }

    ~JQQmlImageTextureFactory() = default;

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
        if ( !QFileInfo::exists( jqicFilePath ) ) { return false; }

        if ( preloadCacheDatas_.contains( jqicFilePath ) ) { return false; }

        auto &preloadCacheData = preloadCacheDatas_[ jqicFilePath ];
        preloadCacheData.mutexForPreload_.reset( new QMutex );
        preloadCacheData.mutexForPreload_->lock();

        // 到新线程去加载，不影响主线程
        QtConcurrent::run(
                    [
                        jqicFilePath,
                        &preloadCacheData
                    ]()
        {
            QFile jqicFile( jqicFilePath );

            if ( !jqicFile.exists() || ( jqicFile.size() < static_cast< qint64 >( sizeof( JQQmlImageInformationHead ) ) ) )
            {
                preloadCacheData.mutexForPreload_->unlock();
                preloadCacheData.mutexForPreload_.clear();
                return;
            }

            if ( !jqicFile.open( QIODevice::ReadOnly ) )
            {
                qDebug() << "open file error:" << jqicFilePath;

                preloadCacheData.mutexForPreload_->unlock();
                preloadCacheData.mutexForPreload_.clear();

                return;
            }

            preloadCacheData.headData = jqicFile.read( sizeof( JQQmlImageInformationHead ) );
            preloadCacheData.imageData = jqicFile.readAll();

            preloadCacheData.mutexForPreload_->unlock();
            preloadCacheData.mutexForPreload_.clear();
        } );

        return true;
    }

private:
    static QMap< QString, PreloadCacheData > preloadCacheDatas_; // jqicFilePath -> PreloadCacheData; 预读取数据的容器

    QString id_;
    QImage image_;
    QByteArray buffer_;
};

QMap< QString, PreloadCacheData > JQQmlImageTextureFactory::preloadCacheDatas_;

// JQQmlImageImageProvider
class JQQmlImageImageProvider: public QQuickImageProvider
{
public:
    JQQmlImageImageProvider():
        QQuickImageProvider( QQmlImageProviderBase::Texture )
    { }

    ~JQQmlImageImageProvider() = default;

    QQuickTextureFactory *requestTexture(const QString &id, QSize *, const QSize &)
    {
        return new JQQmlImageTextureFactory( id );
    }
};

// JQQmlImageManage
QPointer< QQmlApplicationEngine > JQQmlImageManage::qmlApplicationEngine_;
QPointer< QQuickView > JQQmlImageManage::quickView_;
QPointer< JQQmlImageManage > JQQmlImageManage::jqQmlImageManage_;

JQQmlImageManage::JQQmlImageManage():
    mutexForAutoPreloadImage_( new QMutex ),
    listForAutoPreloadImage_( new QStringList )
{
    jqQmlImageManage_ = this;

    if ( !qmlApplicationEngine_.isNull() )
    {
        qmlApplicationEngine_->addImageProvider( "JQQmlImage", new JQQmlImageImageProvider );
    }
    else if ( !quickView_.isNull() )
    {
        quickView_->engine()->addImageProvider( "JQQmlImage", new JQQmlImageImageProvider );
    }
    else
    {
        qDebug() << "JQQmlImageManage::JQQmlImageManage(): error";
    }
}

JQQmlImageManage::~JQQmlImageManage()
{
    jqQmlImageManage_ = nullptr;

    mutexForAutoPreloadImage_->lock();
    saveAutoPreloadImageFileListToFile( *listForAutoPreloadImage_ );
    mutexForAutoPreloadImage_->unlock();
}

void JQQmlImageManage::initialize(QQmlApplicationEngine *qmlApplicationEngine)
{
    qmlApplicationEngine->addImportPath( ":/JQQmlImageQml/" );
    qmlApplicationEngine_ = qmlApplicationEngine;
}

void JQQmlImageManage::initialize(QQuickView *quickView)
{
    quickView->engine()->addImportPath( ":/JQQmlImageQml/" );
    quickView_ = quickView;
}

bool JQQmlImageManage::preload(const QString &imageFilePath)
{
    return JQQmlImageTextureFactory::preload( jqicFilePath( imageFilePath ) );
}

void JQQmlImageManage::autoPreload()
{
    const auto &&list = readAutoPreloadImageFileListToFile();

    for ( const auto &filePath: list )
    {
        preload( filePath );
    }
}

bool JQQmlImageManage::clearAllCache()
{
    return QDir( jqicPath() ).removeRecursively();
}

QString JQQmlImageManage::jqicPath()
{
    if ( !qApp )
    {
        qDebug() << "JQQmlImageManage: qApp is null";
        return { };
    }

    const auto &&cacheLocation = QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
    if ( cacheLocation.isEmpty() )
    {
        qDebug() << "JQQmlImageManage: CacheLocation is empty";
        return { };
    }

    const auto &&buf = QString( "%1/jqqmlimagecache" ).arg( cacheLocation );
    if ( !QFileInfo( buf ).exists() )
    {
        if ( !QDir().mkpath( buf ) )
        {
            qDebug() << "JQQmlImageManage: mkpath error:" << buf;
            return { };
        }
    }

    return buf;
}

QString JQQmlImageManage::jqicFilePath(const QString &imageFilePath)
{
    const auto &&imageFileInfo = QFileInfo( imageFilePath );
    if ( !imageFileInfo.isFile() ) { return { }; }

    QByteArray sumString;

    sumString += imageFilePath;
    sumString += JQQMLIMAGE_VERSION;
    sumString += QByteArray::number( imageFileInfo.lastModified().toMSecsSinceEpoch() );

    const auto &&md5String = QCryptographicHash::hash( sumString, QCryptographicHash::Md5 ).toHex();

    return QString( "%1/jqqmlimagecache/%2.jqic" ).
            arg( QStandardPaths::writableLocation( QStandardPaths::CacheLocation ), md5String.constData() );
}

QPair< JQQmlImageInformationHead, QByteArray > JQQmlImageManage::imageToJqicData(const QImage &image)
{
    QPair< JQQmlImageInformationHead, QByteArray > result;

    if ( image.format() == QImage::Format_Invalid )
    {
        return result;
    }

    if ( ( image.format() != QImage::Format_ARGB32 ) && ( image.format() != QImage::Format_RGB888 ) )
    {
        if ( image.hasAlphaChannel() )
        {
            return imageToJqicData( image.convertToFormat( QImage::Format_ARGB32 ) );
        }
        else
        {
            return imageToJqicData( image.convertToFormat( QImage::Format_RGB888 ) );
        }
    }

    result.first.imageWidth = image.width();
    result.first.imageHeight = image.height();
    result.first.imageFormat = image.format();
    result.first.imageColorCount = image.colorCount();
    result.first.byteIsOrdered = ( image.format() == QImage::Format_ARGB32 ) ? ( 4 ) : ( 3 ) * image.width() == image.bytesPerLine();

//    qDebug() << "result.first.byteIsOrdered:" << result.first.byteIsOrdered;

    // 分析背景色和主要颜色
    {
        // 遍历图片，记录RGB值
        QMap< quint32, int > rgbCountMap;
        for ( quint16 y = 0; y < image.height(); ++y )
        {
            for ( quint16 x = 0; x < image.width(); ++x )
            {
//                qDebug() << image.pixelColor( x, y ) << colorToUint32( image.pixelColor( x, y ) );
//                QThread::sleep( 1 );

                ++rgbCountMap[ colorToUint32( image.pixelColor( x, y ) ) ];
            }
        }

        if ( rgbCountMap.isEmpty() )
        {
            qDebug() << "JQQmlImageManage: rgb count error";
            return result;
        }

        QVector< QPair< quint32, int > > rgbCountVector; // [ { rgb, count }, ... ]
        for ( auto it = rgbCountMap.begin(); it != rgbCountMap.end(); ++it )
        {
            rgbCountVector.push_back( { it.key(), it.value() } );
        }

        // 按照颜色出现次数降序排序
        std::sort(
                    rgbCountVector.begin(),
                    rgbCountVector.end(),
                    [ ](const QPair< quint32, int > &a, const QPair< quint32, int > &b)
                    {
                        return a.second > b.second;
                    }
        );

//        qDebug() << rgbCountVector;

        if ( ( rgbCountVector.size() >= 1 ) &&
           ( ( static_cast< double >( rgbCountVector[ 0 ].second ) / static_cast< double >( image.width() * image.height() ) ) > 0.1 ) )
        {
            result.first.imageHaveBackgroundColor = true;
            result.first.imageBackgroundColor = rgbCountVector[ 0 ].first;
        }

        if ( ( rgbCountVector.size() >= 2 ) &&
           ( ( static_cast< double >( rgbCountVector[ 1 ].second ) / static_cast< double >( image.width() * image.height() ) ) > 0.1 ) )
        {
            result.first.imageHavePrimaryColor = true;
            result.first.imagePrimaryColor = rgbCountVector[ 1 ].first;
        }
    }

//    qDebug() << "result.first.imageHaveBackgroundColor:" << result.first.imageHaveBackgroundColor;
//    qDebug() << "result.first.imageHavePrimaryColor:" << result.first.imageHavePrimaryColor;

    QByteArray backgroundColorByteArray;
    QByteArray primaryColorByteArray;
    QVector< QPair< qint32, QByteArray > > argbSegments; // [ { colorIndexStart, ARGB/RGB data } ]

    // ARGB
    if ( result.first.imageHaveBackgroundColor || result.first.imageHavePrimaryColor )
    {
        QPair< qint32, QByteArray > argbSegment = { -1, { } };

        // 如果有背景色或者主要色，则提取剩余的颜色到 argbSegments
        for ( qint32 colorIndex = 0; colorIndex < ( image.width() * image.height() ); )
        {
            const auto &&currentColor = image.pixelColor( colorIndex % image.width(), colorIndex / image.width() );

//            qDebug() << currentColor << uint32ToColor( result.first.imageBackgroundColor );
//            if ( currentColor == uint32ToColor( result.first.imageBackgroundColor ) )
//            {
//                qDebug() << sameColorDetector( image, colorIndex, result.first.byteIsOrdered );
//            }

            if ( result.first.imageHaveBackgroundColor && ( currentColor == uint32ToColor( result.first.imageBackgroundColor ) ) && ( sameColorDetector( image, colorIndex, result.first.byteIsOrdered ) > 8 ) )
            {
                if ( argbSegment.first != -1 )
                {
                    argbSegments.push_back( argbSegment );
                    argbSegment = { -1, { } };
                }

                ++result.first.imageBackgroundColorSegmentCount;

                const auto &&sameColorCount = sameColorDetector( image, colorIndex, result.first.byteIsOrdered );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 0 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 1 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 2 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 3 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 0 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 1 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 2 ) );
                backgroundColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 3 ) );

                colorIndex += sameColorCount;
            }
            else if ( result.first.imageHavePrimaryColor && ( currentColor == uint32ToColor( result.first.imagePrimaryColor ) ) && ( sameColorDetector( image, colorIndex, result.first.byteIsOrdered ) > 8 ) )
            {
                if ( argbSegment.first != -1 )
                {
                    argbSegments.push_back( argbSegment );
                    argbSegment = { -1, { } };
                }

                ++result.first.imagePrimaryColorSegmentCount;

                const auto &&sameColorCount = sameColorDetector( image, colorIndex, result.first.byteIsOrdered );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 0 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 1 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 2 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &colorIndex ) + 3 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 0 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 1 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 2 ) );
                primaryColorByteArray.append( *( reinterpret_cast< const char * >( &sameColorCount ) + 3 ) );

                colorIndex += sameColorCount;
            }
            else
            {
                if ( argbSegment.first == -1 )
                {
                    argbSegment.first = colorIndex;
                }

                if ( image.hasAlphaChannel() )
                {
                    argbSegment.second.push_back( static_cast< char >( currentColor.blue() ) );
                    argbSegment.second.push_back( static_cast< char >( currentColor.green() ) );
                    argbSegment.second.push_back( static_cast< char >( currentColor.red() ) );
                    argbSegment.second.push_back( static_cast< char >( currentColor.alpha() ) );
                }
                else
                {
                    argbSegment.second.push_back( static_cast< char >( currentColor.red() ) );
                    argbSegment.second.push_back( static_cast< char >( currentColor.green() ) );
                    argbSegment.second.push_back( static_cast< char >( currentColor.blue() ) );
                }

                ++colorIndex;
            }

            if ( result.first.byteIsOrdered && colorIndex && !( colorIndex % image.width() ) )
            {
                if ( argbSegment.first != -1 )
                {
                    argbSegments.push_back( argbSegment );
                    argbSegment = { -1, { } };
                }
            }
        }

        if ( argbSegment.first != -1 )
        {
            argbSegments.push_back( argbSegment );
            argbSegment = { -1, { } };
        }
    }
    else
    {
        // 如果没有背景色或者主要色，则提取全部颜色到 argbSegments
        argbSegments.push_back( { 0, QByteArray( reinterpret_cast< const char * >( image.bits() ), image.byteCount() ) } );
    }

    result.second += backgroundColorByteArray;
    result.second += primaryColorByteArray;

    for ( const auto &argbSegment: argbSegments )
    {
        const qint32 colorIndexStart = argbSegment.first;
        const qint32 &&size = argbSegment.second.size();

//        qDebug() << "colorIndex:" << colorIndexStart << ", size:" << size << ", argbSegment.second.size():" << argbSegment.second.size();

        result.second.append( *( reinterpret_cast< const char * >( &colorIndexStart ) + 0 ) );
        result.second.append( *( reinterpret_cast< const char * >( &colorIndexStart ) + 1 ) );
        result.second.append( *( reinterpret_cast< const char * >( &colorIndexStart ) + 2 ) );
        result.second.append( *( reinterpret_cast< const char * >( &colorIndexStart ) + 3 ) );
        result.second.append( *( reinterpret_cast< const char * >( &size ) + 0 ) );
        result.second.append( *( reinterpret_cast< const char * >( &size ) + 1 ) );
        result.second.append( *( reinterpret_cast< const char * >( &size ) + 2 ) );
        result.second.append( *( reinterpret_cast< const char * >( &size ) + 3 ) );
        result.second.append( argbSegment.second );
    }

    return result;
}

QImage JQQmlImageManage::jqicDataToImage(const JQQmlImageInformationHead &head, const QByteArray &data)
{
    QImage result( head.imageWidth, head.imageHeight, static_cast< QImage::Format >( head.imageFormat ) );

#ifndef QT_NO_DEBUG
    // debug模式填充紫色，防止内存中垃圾数据污染
    result.fill( QColor( "#ff00ff" ) );
#endif

    auto dataIndex = 0;

//    qDebug() << ( head.imageFormat == QImage::Format_ARGB32 );
//    qDebug() << head.imageBackgroundColor << head.imageBackgroundColorSegmentCount;

    // 还原背景色
    for ( auto segmentCount = 0; segmentCount < head.imageBackgroundColorSegmentCount; ++segmentCount )
    {
        qint32 colorIndexStart;
        qint32 sameColorCount;

        *( reinterpret_cast< char * >( &colorIndexStart ) + 0 ) = *( data.data() + dataIndex + 0 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 1 ) = *( data.data() + dataIndex + 1 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 2 ) = *( data.data() + dataIndex + 2 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 3 ) = *( data.data() + dataIndex + 3 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 0 ) = *( data.data() + dataIndex + 4 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 1 ) = *( data.data() + dataIndex + 5 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 2 ) = *( data.data() + dataIndex + 6 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 3 ) = *( data.data() + dataIndex + 7 );

        if ( head.imageFormat == QImage::Format_ARGB32 )
        {
            auto target = reinterpret_cast< quint32 * >( result.bits() + getBitIndexFromColorIndex( result, colorIndexStart, head.byteIsOrdered ) );
            const auto end = target + sameColorCount;

            for ( ; target < end; ++target )
            {
                *target = head.imageBackgroundColor;
            }
        }
        else
        {
            auto target = reinterpret_cast< quint8 * >( result.bits() + getBitIndexFromColorIndex( result, colorIndexStart, head.byteIsOrdered ) );
            const auto end = target + sameColorCount * 3;

            while ( target < end )
            {
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imageBackgroundColor ) + 0 );
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imageBackgroundColor ) + 1 );
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imageBackgroundColor ) + 2 );

//                target += 3;
            }
        }

        dataIndex += 8;
    }

    // 还原主要色
    for ( auto segmentCount = 0; segmentCount < head.imagePrimaryColorSegmentCount; ++segmentCount )
    {
        qint32 colorIndexStart;
        qint32 sameColorCount;

        *( reinterpret_cast< char * >( &colorIndexStart ) + 0 ) = *( data.data() + dataIndex + 0 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 1 ) = *( data.data() + dataIndex + 1 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 2 ) = *( data.data() + dataIndex + 2 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 3 ) = *( data.data() + dataIndex + 3 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 0 ) = *( data.data() + dataIndex + 4 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 1 ) = *( data.data() + dataIndex + 5 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 2 ) = *( data.data() + dataIndex + 6 );
        *( reinterpret_cast< char * >( &sameColorCount ) + 3 ) = *( data.data() + dataIndex + 7 );

        if ( head.imageFormat == QImage::Format_ARGB32 )
        {
            auto target = reinterpret_cast< quint32 * >( result.bits() + getBitIndexFromColorIndex( result, colorIndexStart, head.byteIsOrdered ) );
            const auto end = target + sameColorCount;

            for ( ; target < end; ++target )
            {
                *target = head.imagePrimaryColor;
            }
        }
        else
        {
            auto target = reinterpret_cast< quint8 * >( result.bits() + getBitIndexFromColorIndex( result, colorIndexStart, head.byteIsOrdered ) );
            const auto end = target + sameColorCount * 3;

            while ( target < end )
            {
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imagePrimaryColor ) + 0 );
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imagePrimaryColor ) + 1 );
                *( target++ ) = *( reinterpret_cast< const quint8 * >( &head.imagePrimaryColor ) + 2 );

//                target += 3;
            }
        }

        dataIndex += 8;
    }

    // 还原其他颜色
    while ( dataIndex <= ( data.size() - 8 ) )
    {
        qint32 colorIndexStart;
        qint32 size;

        *( reinterpret_cast< char * >( &colorIndexStart ) + 0 ) = *( data.data() + dataIndex + 0 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 1 ) = *( data.data() + dataIndex + 1 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 2 ) = *( data.data() + dataIndex + 2 );
        *( reinterpret_cast< char * >( &colorIndexStart ) + 3 ) = *( data.data() + dataIndex + 3 );
        *( reinterpret_cast< char * >( &size ) + 0 ) = *( data.data() + dataIndex + 4 );
        *( reinterpret_cast< char * >( &size ) + 1 ) = *( data.data() + dataIndex + 5 );
        *( reinterpret_cast< char * >( &size ) + 2 ) = *( data.data() + dataIndex + 6 );
        *( reinterpret_cast< char * >( &size ) + 3 ) = *( data.data() + dataIndex + 7 );

        memcpy( result.bits() + getBitIndexFromColorIndex( result, colorIndexStart, head.byteIsOrdered ), data.data() + 8 + dataIndex, static_cast< size_t >( size ) );

        dataIndex += 8 + size;
    }

    return result;
}

void JQQmlImageManage::recordImageFilePath(const QString &imageFilePath)
{
    if ( listForAutoPreloadImage_->size() >= 10 ) { return; }

    mutexForAutoPreloadImage_->lock();
    listForAutoPreloadImage_->push_back( imageFilePath );
    mutexForAutoPreloadImage_->unlock();
}

qint32 JQQmlImageManage::sameColorDetector(const QImage &image, const qint32 &colorIndexStart, const bool &byteIsOrdered)
{
    qint32 sameColorCount = 1;
    const auto &&targetColor = image.pixelColor( colorIndexStart % image.width(), colorIndexStart / image.width() );

    if ( byteIsOrdered )
    {
        for ( qint32 colorIndex = colorIndexStart + 1; colorIndex < ( image.width() * image.height() ); ++colorIndex )
        {
            if ( image.pixelColor( colorIndex % image.width(), colorIndex / image.width() ) != targetColor )
            {
                return sameColorCount;
            }

            ++sameColorCount;
        }
    }
    else
    {
        for ( qint32 colorIndex = colorIndexStart + 1; colorIndex < ( colorIndexStart + ( image.width() - colorIndexStart % image.width() ) ); ++colorIndex )
        {
            if ( image.pixelColor( colorIndex % image.width(), colorIndex / image.width() ) != targetColor )
            {
                return sameColorCount;
            }

            ++sameColorCount;
        }
    }

    return sameColorCount;
}

void JQQmlImageManage::saveAutoPreloadImageFileListToFile(const QStringList &imageFilePathList)
{
    auto file = autoPreloadImageFile();
    if ( !file->open( QIODevice::WriteOnly ) )
    {
        qDebug() << "JQQmlImageManage: open file error:" << file->fileName();
        return;
    }

    file->write( imageFilePathList.join( "\n" ).toUtf8() );
    file->waitForBytesWritten( 30 * 1000 );
}

QStringList JQQmlImageManage::readAutoPreloadImageFileListToFile()
{
    auto file = autoPreloadImageFile();

    if ( !file->exists() ) { return { }; }

    if ( !file->open( QIODevice::ReadOnly ) )
    {
        qDebug() << "JQQmlImageManage: open file error:" << file->fileName();
        return { };
    }

    const auto &&rawList = file->readAll().split( '\n' );
    QStringList reply;

    for ( const auto &filePath: rawList )
    {
        reply.push_back( QString::fromUtf8( filePath ) );
    }

    return reply;
}

QSharedPointer< QFile > JQQmlImageManage::autoPreloadImageFile()
{
    return QSharedPointer< QFile >( new QFile( QString( "%1/autopreloadlist" ).arg( JQQmlImageManage::jqicPath() ) ) );
}
