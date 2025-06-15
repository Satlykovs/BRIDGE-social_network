import QtQuick

Row {
    id: root
    spacing: 12
    anchors.margins: 15
    property Window window
    property bool wasMaximized: false

    Repeater {
        model: [
            { "type": "minimize" },
            { "type": "maximize" },
            { "type": "close" }
        ]

        delegate: Rectangle {
            id: button
            width: 24; height: 24; radius: 12
            color: area.pressed ? "rgba(255, 255, 255, 0.1)" : "rgba(255, 255, 255, 0.2)"

            // --- Иконки (оставляем как было) ---
            Rectangle {
                visible: modelData.type === "minimize"
                width: parent.width * 0.5; height: 2
                color: "#FFFFFF"
                anchors.centerIn: parent
            }

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
                id: area
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    if (modelData.type === "minimize") {
                        if (window.visibility === Window.Maximized) {
                            window.showNormal()
                            // Ждем завершения анимации перед минимизацией
                            minimizeTimer.start()
                        } else {
                            window.showMinimized()
                        }
                    }
                    else if (modelData.type === "maximize") {
                        if (window.visibility === Window.Maximized) {
                            window.showNormal()
                        } else {
                            window.showMaximized()
                        }
                    }
                    else if (modelData.type === "close") {
                        Qt.quit()
                    }
                }

                onEntered: button.scale = 1.1
                onExited: button.scale = 1.0
            }
            Behavior on scale { NumberAnimation { duration: 100 } }
        }
    }

    Timer {
        id: minimizeTimer
        interval: 50 // Небольшая задержка для завершения анимации
        onTriggered: {
            window.showMinimized()
        }
    }

    Connections {
        target: window
        function onVisibilityChanged() {
            if (window.visibility === Window.Windowed && wasMaximized) {
                // Восстанавливаем максимизированное состояние после разворачивания
                restoreTimer.start()
            }
        }
    }

    Timer {
        id: restoreTimer
        interval: 50
        onTriggered: {
            if (wasMaximized) {
                root.window.visibility = Window.Maximized
                window.showMaximized()
                wasMaximized = false
            }
        }
    }
}
