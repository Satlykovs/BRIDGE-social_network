import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects


Rectangle {
    id: closeButton
    width: 32
    height: 32
    radius: 16
    anchors.top: parent.top
    anchors.right: parent.right
    anchors.margins: 15
    color: "transparent"

    // Иконка крестика
    Canvas {
        anchors.centerIn: parent
        width: 16
        height: 16

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = closeArea.containsMouse ? "#FF6B6B" : "#FFFFFF"
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(width, height)
            ctx.moveTo(width, 0)
            ctx.lineTo(0, height)
            ctx.stroke()
        }
    }

    // Фон при наведении
    Rectangle {
        id: bg
        anchors.fill: parent
        radius: parent.radius
        color: closeArea.containsMouse ? "#20FFFFFF" : "transparent"
        Behavior on color { ColorAnimation { duration: 150 } }
    }

    // Эффект нажатия
    scale: closeArea.pressed ? 0.9 : 1.0
    Behavior on scale { NumberAnimation { duration: 100 } }

    // Обработка клика
    MouseArea {
        id: closeArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: Qt.quit()
    }

    // Тень
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#40000000"
        shadowBlur: 0.5
        shadowVerticalOffset: 2
        shadowHorizontalOffset: 0
    }
}
