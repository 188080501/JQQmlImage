import QtQuick 2.6
import QtQuick.Window 2.2
import JQQmlImage 1.0

// 从本地加载qml时用的main
Window {
    visible: true
    width: 800
    height: 400
    title: "JQQmlImage"

    Grid {
        id: grid

        // 根据相对路径加载4个本地png图片
        Repeater {
            model: 4

            JQQmlImage {
                width: 200
                height: 200
                cacheImageSource: "../testimages/test" + ( index + 1 ).toString() + ".png"
                cache: false
            }
        }

        // 根据相对路径加载4个本地jpg图片
        Repeater {
            model: 4

            JQQmlImage {
                width: 200
                height: 200
                cacheImageSource: "../testimages/test" + ( index + 1 ).toString() + ".jpg"
                cache: false
            }
        }
    }
}
