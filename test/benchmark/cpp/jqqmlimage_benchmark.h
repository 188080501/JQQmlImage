#ifndef CPP_JQQMLIMAGE_BENCHMARK_H_
#define CPP_JQQMLIMAGE_BENCHMARK_H_

// C++ lib import
#include <set>

// Qt lib import
#include <QObject>

//#define PRIVATEMACRO public
#define PRIVATEMACRO private

class JQQmlImageBenchmark: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageBenchmark )

public:
    JQQmlImageBenchmark() = default;

    ~JQQmlImageBenchmark() = default;

PRIVATEMACRO slots:
    void initTestCase();

    void cleanupTestCase();

    void decodeBenchmark();

    void decodeBenchmark_data();

private:
    std::set< double > decodeTimeResults_;
    std::set< double > decodeSizeResults_;
};

#endif//CPP_JQQMLIMAGE_BENCHMARK_H_
