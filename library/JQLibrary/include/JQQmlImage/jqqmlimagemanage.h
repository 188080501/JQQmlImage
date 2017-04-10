#ifndef __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
#define __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__

// Qt lib import
#include <QObject>
#include <QSharedPointer>
#include <QPointer>

#define JQQMLIMAGEMANAGE_INITIALIZE( carrier ) \
    qmlRegisterType< JQQmlImageManage >( "JQQmlImageManage", 1, 0, "JQQmlImageManage" ); \
    JQQmlImageManage::initialize( &carrier ); \
    JQQmlImageManage::autoPreload();

#define JQQMLIMAGE_VERSION "0.0.8"

class QQuickWindow;
class QQmlApplicationEngine;
class QQuickView;
class QMutex;
class QFile;

class JQQmlImageManage: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageManage )

public:
    JQQmlImageManage();

    ~JQQmlImageManage();

    inline static QPointer< JQQmlImageManage > jqQmlImageManage();

    static void initialize(QQmlApplicationEngine *qmlApplicationEngine);

    static void initialize(QQuickView *quickView);

    static bool preload(const QString &imageFilePath);

    static void autoPreload();

    static bool clearAllCache();

    static QString jqicPath();

    static QString jqicFilePath(const QString &imageFilePath);

    void recordImageFilePath(const QString &imageFilePath);

private:
    static void saveAutoPreloadImageFileListToFile(const QStringList &imageFilePathList);

    static QStringList readAutoPreloadImageFileListToFile();

    static QSharedPointer< QFile > autoPreloadImageFile();

private:
    static QPointer< QQmlApplicationEngine > qmlApplicationEngine_;
    static QPointer< QQuickView > quickView_;
    static QPointer< JQQmlImageManage > jqQmlImageManage_;

    QSharedPointer< QMutex > mutexForAutoPreloadImage_;
    QSharedPointer< QStringList > listForAutoPreloadImage_;
};

// .inc include
#include "jqqmlimagemanage.inc"

#endif//__JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
