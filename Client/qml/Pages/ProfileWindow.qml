import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


import Client.Components

ApplicationWindow {
    visible: false // Сначала скрываем окно
    width: Screen.width
    height: Screen.height
    title: "Профиль пользователя"
    color: "#f5f5f5"
    flags: Qt.Window | Qt.WindowFullScreen

    Component.onCompleted: {
        visible = true // Делаем окно видимым после установки полноэкранного режима
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Аватар пользователя
        Item {
            width: 150
            height: 150
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: avatarBackground
                width: parent.width
                height: parent.height
                radius: width / 2 // Делаем круглый фон
                color: "lightgray" // Цвет фона, если изображение не загружено
                clip: true

                Image {
                    id: avatar
                    source: "/images/avatar.jpg" // Убедитесь, что путь правильный
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    clip: true
                }
            }
        }

        // Имя пользователя
        Label {
            text: "John Doe"
            font.pixelSize: 24
            font.bold: true
            color: "#333"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Описание профиля
        Label {
            text: "Software Developer | Tech Enthusiast"
            font.pixelSize: 16
            color: "#666"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Кнопки действий
        RowLayout {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "Follow"
                font.pixelSize: 14
                background: Rectangle {
                    color: "#007bff"
                    radius: 5
                }
                padding: 10
                onClicked: console.log("Follow button clicked")
            }

            Button {
                text: "Message"
                font.pixelSize: 14
                background: Rectangle {
                    color: "#28a745"
                    radius: 5
                }
                padding: 10
                onClicked: console.log("Message button clicked")
            }
        }
    }
}
