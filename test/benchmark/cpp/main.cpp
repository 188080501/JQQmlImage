// Qt lib import
#include <QCoreApplication>
#include <QtTest>

// JQQmlImage lib import
#include <JQQmlImage>

// Project import
#include "jqqmlimage_benchmark.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "jqicPath:" << JQQmlImageManage::jqicPath();

    JQQmlImageBenchmark jqQmlImageBenchmark;

    return QTest::qExec( &jqQmlImageBenchmark, argc, argv );
}

