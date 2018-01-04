import QtQuick 2.6
import QtQuick.Window 2.2
import JQQmlImage 1.0

// 从qrc加载qml时用的main
Window {
    visible: true
    width: 800
    height: 400
    title: "JQQmlImage"

    Grid {
        id: grid

        // 根据绝对路径加载4个本地png图片
        Repeater {
            model: 4

            JQQmlImage {
                width: 200
                height: 200
                cacheImageSource: "file:///Users/jason/Desktop/JQQmlImage/testimages/test" + ( index + 1 ).toString() + ".png"
                cache: false
            }
        }

        // 根据绝对路径加载3个本地jpg图片
        Repeater {
            model: 3

            JQQmlImage {
                width: 200
                height: 200
                cacheImageSource: "file:///Users/jason/Desktop/JQQmlImage/testimages/test" + ( index + 1 ).toString() + ".jpg"
                cache: false
            }
        }

        // 加载1个qrc中的jpg图片
        JQQmlImage {
            width: 200
            height: 200
            cacheImageSource: "qrc:/testimages/test4.jpg"
            cache: false
        }
    }
}
