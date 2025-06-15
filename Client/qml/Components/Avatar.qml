import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    property alias source: image.source
    width: 160
    height: 160

    layer.enabled: true
    layer.effect: DropShadow {
        radius: 16
        samples: 24
        color: "#80000000"
        verticalOffset: 4
    }

    OpacityMask {
        anchors.fill: parent
        source: Image {
            id: image
            fillMode: Image.PreserveAspectCrop
        }
        maskSource: Rectangle {
            width: 160
            height: 160
            radius: 80
        }
    }
}