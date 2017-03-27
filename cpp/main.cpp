// Qt lib import
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

// JQLibrary lib import
#include <JQQmlImage>

int main(int argc, char *argv[])
{
    QGuiApplication app( argc, argv );

    QQmlApplicationEngine engine;

    JQQMLIMAGE_INITIALIZE( engine );

    qDebug() << "JQImageCache path:" << JQQmlImageManage::jqicPath();

    // 对特定图片预加载，进一步提升加载速度，后期要改成自动预加载
    // 加载在单独的线程进行，不影响主线程执行
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test3.png" );
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test3.jpg" );

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    return app.exec();
}
