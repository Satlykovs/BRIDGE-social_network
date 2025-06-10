import QtQuick

Row {
    id: root
    spacing: 12
    anchors.margins: 15
    property Window window

    // Repeater создаст три кнопки на основе своей модели
    Repeater {
        // МОДЕЛЬ ТЕПЕРЬ НАХОДИТСЯ ЗДЕСЬ, ГДЕ И ДОЛЖНА БЫТЬ
        model: [
            { "type": "minimize" },
            { "type": "maximize" },
            { "type": "close" }
        ]

        // Делегат (шаблон) для каждой кнопки
        delegate: Rectangle {
            id: button
            width: 24; height: 24; radius: 12
            color: area.pressed ? "rgba(255, 255, 255, 0.1)" : "rgba(255, 255, 255, 0.2)"

            // --- Рисуем иконку в зависимости от типа ---

            // Иконка "минус"
            Rectangle {
                visible: modelData.type === "minimize"
                width: parent.width * 0.5; height: 2
                color: "#FFFFFF"
                anchors.centerIn: parent
            }

            // Иконка "квадрат"
            Item {
                visible: modelData.type === "maximize"
                anchors.fill: parent
                Rectangle {
                    width: parent.width * 0.45; height: parent.height * 0.45
                    color: "transparent"
                    border.color: "#FFFFFF"; border.width: 1.5
                    anchors.centerIn: parent
                }
                Rectangle {
                    visible: root.window && root.window.visibility === Window.Maximized
                    width: parent.width * 0.45; height: parent.height * 0.45
                    x: parent.width * 0.35; y: parent.height * 0.2
                    color: "transparent"
                    border.color: "#FFFFFF"; border.width: 1.5
                }
            }

            // Иконка "крестик"
            Item {
                visible: modelData.type === "close"
                anchors.fill: parent
                Rectangle {
                    width: parent.width * 0.5; height: 1.5
                    color: "#FFFFFF"; rotation: 45
                    anchors.centerIn: parent
                }
                Rectangle {
                    width: parent.width * 0.5; height: 1.5
                    color: "#FFFFFF"; rotation: -45
                    anchors.centerIn: parent
                }
            }

            MouseArea {
                id: area; anchors.fill: parent; hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                // ЛОГИКА КЛИКА ТЕПЕРЬ ЗДЕСЬ, ВНУТРИ MOUSEAREA
                onClicked: {
                    if (modelData.type === "minimize") {
                        root.window.showMinimized()
                    } else if (modelData.type === "maximize") {
                        if (root.window.visibility === Window.Maximized) {
                            root.window.showNormal()
                        } else {
                            root.window.showMaximized()
                        }
                    } else if (modelData.type === "close") {
                        Qt.quit()
                    }
                }

                onEntered: button.scale = 1.1
                onExited: button.scale = 1.0
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
        }
    }
}
