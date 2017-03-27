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

    // 对数据预加载，进一步提升加载速度，后期要改成自动预加载
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test1.png" );
    JQQmlImageManage::preload( "/Users/jason/Desktop/JQQmlImage/testimages/test1.jpg" );

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    return app.exec();
}
