#ifndef CPP_JQQMLIMAGE_OVERALLTEST_H_
#define CPP_JQQMLIMAGE_OVERALLTEST_H_

// Qt lib import
#include <QObject>

//#define PRIVATEMACRO public
#define PRIVATEMACRO private

class JQQmlImageOverallTest: public QObject
{
     Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageOverallTest )

public:
    JQQmlImageOverallTest() = default;

    ~JQQmlImageOverallTest() = default;

PRIVATEMACRO slots:
    void initTestCase();

    void cleanupTestCase();

    void dataIntegrity();

    void dataIntegrity_data();
};

#endif//CPP_JQQMLIMAGE_OVERALLTEST_H_
