/*
    This file is part of JQLibrary

    Copyright: Jason

    Contact email: 188080501@qq.com

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 or version 3 as published by the Free
    Software Foundation and appearing in the file LICENSE.LGPLv21 and
    LICENSE.LGPLv3 included in the packaging of this file. Please review the
    following information to ensure the GNU Lesser General Public License
    requirements will be met: https://www.gnu.org/licenses/lgpl.html and
    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef JQQMLIMAGE_JQQMLIMAGEMANAGE_H_
#define JQQMLIMAGE_JQQMLIMAGEMANAGE_H_

// Qt lib import
#include <QObject>
#include <QSharedPointer>
#include <QPointer>
#include <QImage>
#include <QColor>

#define JQQMLIMAGEMANAGE_INITIALIZE( carrier ) \
    qmlRegisterType< JQQmlImageManage >( "JQQmlImageManage", 1, 0, "JQQmlImageManage" ); \
    JQQmlImageManage::initialize( &carrier ); \
    JQQmlImageManage::autoPreload();

#define JQQMLIMAGE_VERSION "1.1"

class QQuickWindow;
class QQmlApplicationEngine;
class QQuickView;
class QMutex;
class QFile;

#pragma pack(push)
#pragma pack(8)
struct JQQmlImageInformationHead
{
    qint32 imageWidth = 0;
    qint32 imageHeight = 0;
    qint32 imageFormat = 0;
    qint32 imageColorCount = 0;
    bool byteIsOrdered = false;

    bool imageHaveFirstPrimaryColor = false;
    quint32 imageFirstPrimaryColor = 0;
    qint32 imageFirstPrimaryColorSegmentCount = 0;

    bool imageHaveSecondPrimaryColor = false;
    quint32 imageSecondPrimaryColor = 0;
    qint32 imageSecondPrimaryColorSegmentCount = 0;
};
#pragma pack(pop)

class JQQmlImageManage: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageManage )
    Q_PROPERTY( qreal devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio )

public:
    JQQmlImageManage();

    ~JQQmlImageManage();

    inline static QPointer< JQQmlImageManage > jqQmlImageManage();

    static inline bool enableCacheFeature();

    static inline void setEnableCacheFeature(const bool &newValue);

    static inline bool cachePathIsWritable();

    static inline qreal devicePixelRatio();

    static inline void setDevicePixelRatio(const qreal &newValue);

    static inline QStringList extraSelectors();

    static inline void setExtraSelectors(const QStringList &newValue);


    static void initialize(QQmlApplicationEngine *qmlApplicationEngine);

    static void initialize(QQuickView *quickView);

    static bool preload(const QString &imageFilePath);

    static void autoPreload();

    static bool clearAllCache();

    static QString jqicPath();

    static QString jqicFilePath(const QString &imageFilePath);

    static QPair< JQQmlImageInformationHead, QByteArray > imageToJqicData(const QImage &image);

    static QImage jqicDataToImage(const JQQmlImageInformationHead &head, const QByteArray &data);

    void recordImageFilePath(const QString &imageFilePath);

private:
    inline static QColor uint32ToColor(const quint32 &color);

    inline static quint32 colorToUint32(const QColor &color);

    inline static qint32 getBitIndexFromColorIndex(const QImage &image, const qint32 &colorIndex, const bool &byteIsOrdered);

    static void refreshCachePathIsWritable();

    static qint32 sameColorDetector(const QImage &image, const qint32 &colorIndexStart, const bool &byteIsOrdered);

    static void saveAutoPreloadImageFileListToFile(const QStringList &imageFilePathList);

    static QStringList readAutoPreloadImageFileListToFile();

    static QSharedPointer< QFile > autoPreloadImageFile();

private:
    static QPointer< QQmlApplicationEngine > qmlApplicationEngine_;
    static QPointer< QQuickView > quickView_;
    static QPointer< JQQmlImageManage > jqQmlImageManage_;

    static bool enableCacheFeature_;
    static bool cachePathIsWritable_;
    static qreal devicePixelRatio_;
    static QStringList extraSelectors_;

    QSharedPointer< QMutex > preloadImageMutex_;
    QSharedPointer< QStringList > autoPreloadImageFilePathList_;
};

// .inc include
#include "jqqmlimagemanage.inc"

#endif//JQQMLIMAGE_JQQMLIMAGEMANAGE_H_
