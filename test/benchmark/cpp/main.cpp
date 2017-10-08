// Qt lib import
#include <QCoreApplication>
#include <QtTest>

// Project import
#include "jqqmlimage_benchmark.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    JQQmlImageBenchmark jqQmlImageBenchmark;

    return QTest::qExec( &jqQmlImageBenchmark, argc, argv );
}

