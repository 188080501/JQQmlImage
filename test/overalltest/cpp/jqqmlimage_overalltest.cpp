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

void JQQmlImageOverallTest::dataIntegrity()
{
    QFETCH( QString, imageFilePath );

    QImage sourceImage( imageFilePath );
    QCOMPARE( sourceImage.isNull(), false );

    const auto &&jqicData = JQQmlImageManage::imageToJqicData( sourceImage );
    qDebug() << "jqicData.second.size():" << jqicData.second.size();

    const auto &&targetImage = JQQmlImageManage::jqicDataToImage( jqicData.first, jqicData.second );
//    targetImage.save( "/Users/jason/Desktop/test.png", "PNG" );

    if ( sourceImage.hasAlphaChannel() )
    {
        QCOMPARE( sourceImage.convertToFormat( QImage::Format_ARGB32 ), targetImage.convertToFormat( QImage::Format_ARGB32 ) );
    }
    else
    {
        QCOMPARE( sourceImage.convertToFormat( QImage::Format_RGB888 ), targetImage.convertToFormat( QImage::Format_RGB888 ) );
    }
}

void JQQmlImageOverallTest::dataIntegrity_data()
{
    QTest::addColumn< QString >( "imageFilePath" );

    QTest::newRow( "test1.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test1.png" );
    QTest::newRow( "test1.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test1.jpg" );
    QTest::newRow( "test2.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test2.png" );
    QTest::newRow( "test2.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test2.jpg" );
    QTest::newRow( "test3.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test3.png" );
    QTest::newRow( "test3.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test3.jpg" );
    QTest::newRow( "test4.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test4.png" );
    QTest::newRow( "test4.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test4.jpg" );
    QTest::newRow( "test5.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test5.png" );
    QTest::newRow( "test6.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test6.png" );
    QTest::newRow( "test7.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test7.png" );
    QTest::newRow( "test8.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test8.png" );
    QTest::newRow( "test9.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test9.png" );
    QTest::newRow( "test10.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test10.png" );
    QTest::newRow( "test11.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test11.jpg" );
    QTest::newRow( "test12.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test12.png" );
    QTest::newRow( "test13.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test13.png" );
    QTest::newRow( "test14.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test14.png" );
    QTest::newRow( "test15.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test15.png" );
    QTest::newRow( "test16.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test16.png" );
    QTest::newRow( "test17.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test17.png" );
    QTest::newRow( "test18.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test18.jpg" );
    QTest::newRow( "test19.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test19.jpg" );
    QTest::newRow( "test20.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test20.png" );
    QTest::newRow( "test21.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test21.png" );
    QTest::newRow( "test22.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test22.png" );
    QTest::newRow( "test23.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test23.png" );
    QTest::newRow( "test24.1.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test24.1.png" );
    QTest::newRow( "test24.2.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test24.2.png" );
    QTest::newRow( "test24.3.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test24.3.png" );
    QTest::newRow( "test24.4.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test24.4.png" );
    QTest::newRow( "test25.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test25.png" );
    QTest::newRow( "test26.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test26.png" );
    QTest::newRow( "test27.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test27.png" );
    QTest::newRow( "test28.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test28.png" );
    QTest::newRow( "test29.jpg" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test29.jpg" );
    QTest::newRow( "test30.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test30.png" );
    QTest::newRow( "test31.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test31.png" );
    QTest::newRow( "test32.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test32.png" );
    QTest::newRow( "test33.png" ) << QString( "%1/%2" ).arg( TESTIMAGES_PATH, "test33.png" );
}
