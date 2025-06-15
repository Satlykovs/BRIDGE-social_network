import QtQuick
import QtQuick.Effects

Image {
    width: 120
    height: 120
    source: "qrc:/images/bridge_logo.png"
    fillMode: Image.PreserveAspectFit

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#80FFFFFF"
        shadowBlur: 0.8
        shadowOpacity: 0.5
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 0
        autoPaddingEnabled: true
    }

    SequentialAnimation on y {
        running: true
        loops: Animation.Infinite
        NumberAnimation { duration: 1500; easing.type: Easing.InOutQuad; from: y; to: y-15 }
        NumberAnimation { duration: 1500; easing.type: Easing.InOutQuad; from: y-15; to: y }
    }
}
