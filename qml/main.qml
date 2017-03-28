import QtQuick 2.8
import QtQuick.Window 2.2
import JQQmlImage 1.0

Window {
    visible: true
    width: 800
    height: 400
    title: "JQQmlImage"

    Grid {
        id: grid

        Repeater {
            model: 4

            JQQmlImage {
                id: image1
                width: 200
                height: 200
                cacheImageSource: "file:///Users/jason/Desktop/JQQmlImage/testimages/test" + ( index + 1 ).toString() + ".png"
                cache: false
            }
        }

        Repeater {
            model: 4

            JQQmlImage {
                id: image2
                width: 200
                height: 200
                cacheImageSource: "file:///Users/jason/Desktop/JQQmlImage/testimages/test" + ( index + 1 ).toString() + ".jpg"
                cache: false
            }
        }
    }
}
