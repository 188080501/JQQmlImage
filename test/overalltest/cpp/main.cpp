// Qt lib import
#include <QCoreApplication>
#include <QtTest>

// Project import
#include "jqqmlimage_overalltest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    JQQmlImageOverallTest jqNetworkTest;

    return QTest::qExec( &jqNetworkTest, argc, argv );
}
