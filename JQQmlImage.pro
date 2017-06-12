TEMPLATE = app

QT += qml quick

CONFIG += c++11

include( $$PWD/library/JQLibrary/JQLibrary.pri )
include( $$PWD/library/JQLibrary/JQQmlImage.pri )

SOURCES += \
    $$PWD/cpp/main.cpp

RESOURCES += \
    $$PWD/qml/qml.qrc \
    testimages/testimages.qrc

OTHER_FILES += \
    $$PWD/qml/main2.qml
