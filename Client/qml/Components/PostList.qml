import QtQuick
import QtQuick.Controls

import Client.Components

ListView {
    id: postsListView
    spacing: 20
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    delegate: Post {
        width: Math.min(340, parent.width - 40)
        postData: model
    }

    Label {
        anchors.centerIn: parent
        visible: count === 0
        text: "Публикаций пока нет"
        color: "#707090"
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 16
        padding: 30
    }
}
