// Qt lib import
#include <QCoreApplication>
#include <QtTest>

// Project import
#include "jqqmlimage_overalltest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    JQQmlImageOverallTest jqQmlImageOverallTest;

    return QTest::qExec( &jqQmlImageOverallTest, argc, argv );
}
