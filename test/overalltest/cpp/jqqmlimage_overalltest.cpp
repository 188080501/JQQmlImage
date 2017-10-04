#include "jqqmlimage_overalltest.h"

// Qt lib import
#include <QtTest>
#include <QTime>
#include <QtConcurrent>

// Project lib import
#include <JQQmlImage>

void JQQmlImageOverallTest::initTestCase()
{ }

void JQQmlImageOverallTest::cleanupTestCase()
{ }

void JQQmlImageOverallTest::test1()
{
    JQQmlImageManage::imageToJqicData( QImage( "/Users/Jason/Desktop/JQQmlImage/testimages/test12.png" ) );
}
