import QtQuick
import QtQuick.Effects


Rectangle {
    radius: 25
    layer.enabled: true
    signal onCloseClicked

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#FF6B6B" }
        GradientStop { position: 0.5; color: "#6C5B7B" }
        GradientStop { position: 1.0; color: "#4ECDC4" }
    }

    ParticlesRepeater {
        anchors.fill: parent
    }

    CloseButton {
        id: closeButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15
    }
}
