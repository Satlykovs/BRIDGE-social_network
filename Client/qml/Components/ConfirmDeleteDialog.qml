import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    width: Math.min(400, parent.width * 0.9)
    height: Math.min(150, parent.height * 0.3)
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    padding: 0

    property int postId: -1
    signal confirmed(int postId)

    background: Rectangle {
        color: "#2A2A3E"
        radius: 12
        border.color: "#3A3A4E"
        border.width: 1
    }

    contentItem: ColumnLayout {
        spacing: 15
        width: parent.width

        Label {
            text: "Удаление поста"
            color: "white"
            font.bold: true
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10
        }

        Label {
            text: "Вы уверены, что хотите удалить этот пост?"
            color: "#B0B0D0"
            font.pixelSize: 14
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            spacing: 15
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 15

            Button {
                text: "Отмена"
                implicitWidth: 100
                background: Rectangle {
                    color: "#3A3A4E"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.close()
            }

            Button {
                text: "Удалить"
                implicitWidth: 100
                background: Rectangle {
                    color: "#FF5555"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    root.confirmed(root.postId)
                    root.close()
                }
            }
        }
    }
}
