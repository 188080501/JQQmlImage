#ifndef __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
#define __JQQMLIMAGE_JQQMLIMAGEMANAGE_H__

// Qt lib import
#include <QObject>
#include <QPointer>

#define JQQMLIMAGEMANAGE_INITIALIZE( qmlApplicationEngine ) \
    qmlRegisterType< JQQmlImageManage >( "JQQmlImageManage", 1, 0, "JQQmlImageManage" ); \
    JQQmlImageManage::setQmlApplicationEngine( &qmlApplicationEngine );

#define JQQMLIMAGE_VERSION "0.0.2"

class QQuickWindow;
class QQmlApplicationEngine;

class JQQmlImageManage: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageManage )

public:
    JQQmlImageManage();

    ~JQQmlImageManage() = default;

    static inline void setQmlApplicationEngine(const QPointer< QQmlApplicationEngine > &qmlApplicationEngine);

    static inline QPointer< QQmlApplicationEngine > qmlApplicationEngine();

    static bool preload(const QString &imageFilePath);

    static bool clearAllCache();

    static QString jqicPath();

    static QString jqicFilePath(const QString &imageFilePath);

private:
    static QPointer< QQmlApplicationEngine > qmlApplicationEngine_;
};

// .inc include
#include "jqqmlimagemanage.inc"

#endif//__JQQMLIMAGE_JQQMLIMAGEMANAGE_H__
