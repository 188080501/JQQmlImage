TEMPLATE = app

QT += qml quick

CONFIG += c++11

include( $$PWD/library/JQLibrary/JQLibrary.pri )

SOURCES += \
    $$PWD/cpp/main.cpp

RESOURCES += \
    $$PWD/qml/qml.qrc
