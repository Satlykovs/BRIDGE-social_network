import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Rectangle {
    id: customButton
    Layout.alignment: Qt.AlignHCenter
    width: 200
    height: 50
    radius: 10
    color: "#FF6B6B" // Базовый цвет

    property string text: "Log in"



    signal clicked()

    // Текст кнопки
    Text {
        text: customButton.text
        anchors.centerIn: parent
        color: "white"
        font {
            pixelSize: 16
            family: "Montserrat"
            weight: Font.Bold
        }
    }

    // Тень
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#40000000"
        shadowBlur: 0.5
        shadowOpacity: 0.25
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 0
    }

    // Состояния
    states: [
        State {
            name: "HOVERED"
            PropertyChanges {
                target: customButton
                color: "#FF8888" // Светло-красный
                scale: 0.98
            }
        },
        State {
            name: "PRESSED"
            PropertyChanges {
                target: customButton
                color: "#CC5555" // Темно-красный
                scale: 0.95
            }
        }
    ]

    // Обработчики
    HoverHandler {
        onHoveredChanged: {
            if (hovered) {
                customButton.state = "HOVERED"
            } else {
                customButton.state = ""
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            customButton.state = "PRESSED"
            customButton.clicked()
        }
        onReleased: customButton.state = "HOVERED"
        onExited: customButton.state = ""
    }

    // Анимации
    transitions: Transition {
        ColorAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            properties: "scale"
            duration: 100
            easing.type: Easing.OutQuad
        }
    }
}
