import QtQuick 2.15
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root
    height: 48
    radius: Styles.cornerRadius
    gradient: gradient

    property alias text: label.text
    property Gradient gradient

    Label {
        id: label
        anchors.centerIn: parent
        color: "white"
        font: Styles.mainFont
        font.pixelSize: 16
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onPressed: scaleAnim.start()
        onEntered: parent.opacity = 0.9
        onExited: parent.opacity = 1
    }

    ScaleAnimator {
        id: scaleAnim
        target: root
        from: 1
        to: 0.95
        duration: 100
    }

    layer.enabled: true
    layer.effect: DropShadow {
        radius: 8
        samples: 16
        color: "#20000000"
    }
}
