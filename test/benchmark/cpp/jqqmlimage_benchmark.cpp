#include "jqqmlimage_benchmark.h"

// Qt lib import
#include <QtTest>
#include <QTime>
#include <QtConcurrent>

// Project lib import
#include <JQQmlImage>

void JQQmlImageBenchmark::test1()
{
    QTime time;
    time.start();

    QFile file( "/Users/jason/Desktop/JQQmlImage/testimages/test7.png" );
    file.open( QIODevice::ReadOnly );
    const auto &&data = file.readAll();

    for ( auto i = 0; i < 1000; ++i )
    {
        QImage().loadFromData( data, "PNG" );
    }

    qDebug() << time.elapsed();

    const auto &&jqicData = JQQmlImageManage::imageToJqicData( QImage( "/Users/jason/Desktop/JQQmlImage/testimages/test7.png" ) );
    qDebug() << "jqicData.second.size():" << jqicData.second.size();

    time.restart();

    for ( auto i = 0; i < 1000; ++i )
    {
        JQQmlImageManage::jqicDataToImage( jqicData.first, jqicData.second );
    }

    qDebug() << time.elapsed();
}
