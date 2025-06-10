import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Client.Components

Rectangle {
    id: root
    anchors.fill: parent
    visible: false
    color: "#1E1E2E"

    property var postsModel: ListModel {
        Component.onCompleted: {
            append({
                "id": 1,
                "text": "Живу с арабом! Весело, у него калаш, и он бухой",
                "imageUrl": "qrc:/images/zabuhal.jpg",
                "likes": 24,
                "date": "2023-05-15",
                "liked": false
            });
            append({
                "id": 2,
                "text": "Мой новый проект почти готов к запуску...",
                "imageUrl": "",
                "likes": 12,
                "date": "2023-05-10",
                "liked": true
            });
            append({
                "id": 3,
                "text": "Разрабатываю новый интерфейс для приложения",
                "imageUrl": "",
                "likes": 8,
                "date": "2023-05-05",
                "liked": false
            });
            append({
                "id": 4,
                "text": "Жесточайше забухал",
                "imageUrl": "qrc:/images/zabuhal2.jpg",
                "likes": 100001,
                "date": "2025-06-10",
                "liked": false
            });
        }
    }

    Column {
        anchors.fill: parent
        spacing: 0

        PostHeader {
            id: header
            width: parent.width
            onBackClicked: root.visible = false
        }

        PostList {
            width: parent.width
            height: parent.height - header.height
            model: root.postsModel
        }
    }
}
