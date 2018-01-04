// Qt lib import
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QThread>

// JQLibrary lib import
#include <JQQmlImage>

int main(int argc, char *argv[])
{

    QGuiApplication app( argc, argv );

    // 删除本地缓存，如果要测试没有缓存的情况可以启用这一行
//    JQQmlImageManage::clearAllCache();

    // 若不需要写入新的缓存，可以启用这一行
//    JQQmlImageManage::setEnableCacheFeature( false );

    // 打印缓存存储路径
    qDebug() << "JQImageCache path:" << JQQmlImageManage::jqicPath();

    QQmlApplicationEngine engine;

    // 初始化，这是必须的
    JQQMLIMAGE_INITIALIZE( engine );

    // 从qrc加载qml
    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    // 从本地加载qml
//    engine.load( QString( "%1/main2.qml" ).arg( MAIN2_QML_PATH ) );

    return app.exec();
}
