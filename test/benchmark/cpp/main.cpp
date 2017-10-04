// Qt lib import
#include <QCoreApplication>
#include <QtTest>

// Project import
#include "jqqmlimage_benchmark.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    JQQmlImageBenchmark benchmark;

    qDebug() << "----- test1 start -----";
    benchmark.test1();
    qDebug() << "----- test1 end -----";
}

