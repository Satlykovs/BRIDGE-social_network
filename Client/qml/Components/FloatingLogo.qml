import QtQuick
import Qt5Compat.GraphicalEffects

Image {
    width: 120
    height: 120
    source: "qrc:/images/bridge_logo.png"
    fillMode: Image.PreserveAspectFit

    layer.enabled: true
    layer.effect: DropShadow {
        transparentBorder: true
        radius: 16
        samples: 32
        color: "#80FFFFFF"
    }

    SequentialAnimation on y {
        running: true
        loops: Animation.Infinite
        NumberAnimation { duration: 1500; easing.type: Easing.InOutQuad; from: y; to: y-15 }
        NumberAnimation { duration: 1500; easing.type: Easing.InOutQuad; from: y-15; to: y }
    }
}
