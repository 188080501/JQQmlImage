import QtQuick 2.6
import JQQmlImage 1.0

Image {
    source: "image://JQQmlImage/" + cacheImageSource

    property url cacheImageSource

    readonly property var nothing: JQQmlImageGlobal.nothing
}
