#ifndef CPP_JQQMLIMAGE_BENCHMARK_H_
#define CPP_JQQMLIMAGE_BENCHMARK_H_

// Qt lib import
#include <QObject>

class JQQmlImageBenchmark: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( JQQmlImageBenchmark )

public:
    JQQmlImageBenchmark() = default;

    ~JQQmlImageBenchmark() = default;

    void test1();
};

#endif//CPP_JQQMLIMAGE_BENCHMARK_H_
