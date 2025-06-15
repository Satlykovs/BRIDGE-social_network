import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: addButtonContainer
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: 20
    width: 60
    height: 60
    visible: root.visible

    property var editPostDialog

    signal onClicked()

    // Основная кнопка
    Rectangle {
        id: addButton
        anchors.fill: parent
        radius: width / 2
        color: "#89B4FA"

        // Эффект тени
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            horizontalOffset: 0
            verticalOffset: 2
            radius: 6
            samples: 13
            color: "#40000000"
        }

        // Ripple эффект
        Rectangle {
            id: ripple
            property real size: 0
            property point origin: Qt.point(0, 0)
            width: size
            height: size
            radius: size / 2
            x: origin.x - size / 2
            y: origin.y - size / 2
            color: Qt.rgba(1, 1, 1, 0.2)
            opacity: 0
            visible: false
        }

        // Иконка плюса
        Text {
            text: "+"
            font.pixelSize: 32
            font.weight: Font.Light
            color: "white"
            anchors.centerIn: parent

            rotation: editPostDialog.visible ? 45 : 0
            Behavior on rotation {
                NumberAnimation {
                    duration: 300
                    easing.type: Easing.OutBack
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: function(mouse) {
                // Запускаем ripple-эффект
                ripple.origin = Qt.point(mouse.x, mouse.y)
                ripple.size = 0
                ripple.opacity = 1
                ripple.visible = true
                rippleAnimation.start()

                addButtonContainer.onClicked()
                // Открываем диалог
                editPostDialog.isNewPost = true
                editPostDialog.postText = ""
                editPostDialog.imageUrl = ""
                editPostDialog.open()
            }

            onPressed: {
                addButton.scale = 0.95
            }

            onReleased: {
                addButton.scale = 1.0
            }

            onEntered: {
                hoverAnimation.start()
            }

            onExited: {
                resetAnimation.start()
            }
        }

        // Анимация ripple-эффекта
        SequentialAnimation {
            id: rippleAnimation
            ParallelAnimation {
                NumberAnimation {
                    target: ripple
                    property: "size"
                    to: Math.max(addButton.width, addButton.height) * 1.5
                    duration: 500
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    target: ripple
                    property: "opacity"
                    to: 0
                    duration: 500
                }
            }
            ScriptAction { script: ripple.visible = false }
        }

        // Анимация наведения
        ParallelAnimation {
            id: hoverAnimation
            NumberAnimation {
                target: addButton
                property: "scale"
                to: 1.05
                duration: 150
                easing.type: Easing.OutQuad
            }
            ColorAnimation {
                target: addButton
                property: "color"
                to: Qt.lighter("#89B4FA", 1.1)
                duration: 150
            }
        }

        // Анимация сброса состояния
        ParallelAnimation {
            id: resetAnimation
            NumberAnimation {
                target: addButton
                property: "scale"
                to: 1.0
                duration: 150
                easing.type: Easing.OutQuad
            }
            ColorAnimation {
                target: addButton
                property: "color"
                to: "#89B4FA"
                duration: 150
            }
        }
    }

    // Подсказка при наведении
    ToolTip {
        visible: mouseArea.containsMouse
        text: "Создать новый пост"
        delay: 500
        timeout: 1500
    }
}
