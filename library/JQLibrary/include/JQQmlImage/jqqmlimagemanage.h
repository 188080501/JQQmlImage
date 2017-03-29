#ifndef __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
#define __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__

// Qt lib import
#include <QObject>
#include <QPointer>

#define JQQMLIMAGEMANAGE_INITIALIZE( carrier ) \
    qmlRegisterType< JQQmlImageManage >( "JQQmlImageManage", 1, 0, "JQQmlImageManage" ); \
    JQQmlImageManage::initialize( &carrier );

#define JQQMLIMAGE_VERSION "0.0.5"

class QQuickWindow;
class QQmlApplicationEngine;
class QQuickView;

class JQQmlImageManage: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageManage )

public:
    JQQmlImageManage();

    ~JQQmlImageManage() = default;

    static void initialize(QQmlApplicationEngine *qmlApplicationEngine);

    static void initialize(QQuickView *quickView);

    static bool preload(const QString &imageFilePath);

    static bool clearAllCache();

    static QString jqicPath();

    static QString jqicFilePath(const QString &imageFilePath);

private:
    static QPointer< QQmlApplicationEngine > qmlApplicationEngine_;
    static QPointer< QQuickView > quickView_;
};

// .inc include
#include "jqqmlimagemanage.inc"

#endif//__JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
