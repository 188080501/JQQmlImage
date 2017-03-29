// Qt lib import
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

// JQLibrary lib import
#include <JQQmlImage>

int main(int argc, char *argv[])
{
    QGuiApplication app( argc, argv );

    // 删除本地缓存，如果要测试没有缓存的情况可以启用这一行
//    JQQmlImageManage::clearAllCache();

    // 打印缓存存储路径
    qDebug() << "JQImageCache path:" << JQQmlImageManage::jqicPath();

    QQmlApplicationEngine engine;

    // 初始化，这是必须的
    JQQMLIMAGE_INITIALIZE( engine );

    // 对特定图片预加载，进一步提升加载速度，后期要改成自动预加载
    // 加载在单独的线程进行，不影响主线程执行
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test3.png" );
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test3.jpg" );

    // 从qrc加载qml
    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    // 从本地加载qml
//    engine.load( "../../../../JQQmlImage/qml/main2.qml" );

    return app.exec();
}
