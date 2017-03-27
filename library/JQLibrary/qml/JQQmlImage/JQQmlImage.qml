import QtQuick 2.8
import "."

Image {
    id: image
    source: "image://JQQmlImage/" + cacheImageSource

    property url cacheImageSource

    readonly property var nothing: JQQmlImageGlobal.nothing
}
