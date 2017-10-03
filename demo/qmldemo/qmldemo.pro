TEMPLATE = app

QT += qml quick

CONFIG += c++11

include( $$PWD/../../sharedlibrary/JQLibrary/JQQmlImage.pri )

SOURCES *= \
    $$PWD/cpp/*.cpp

RESOURCES += \
    $$PWD/qml/*.qrc \
    $$PWD/../../testimages/testimages.qrc

OTHER_FILES += \
    $$PWD/qml/main2.qml
