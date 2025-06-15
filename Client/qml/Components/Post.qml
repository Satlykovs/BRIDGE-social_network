import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

import Client.Components

Rectangle {
    id: root
    property var postData
    width: Math.min(340, parent.width - 40)
    implicitHeight: postContent.implicitHeight
    radius: 12
    color: "#2A2A3E"
    border.color: "#3A3A4E"
    border.width: 1
    anchors.horizontalCenter: parent?.horizontalCenter ?? undefined

    // Сигналы для обработки действий
    signal editClicked(var postData)
    signal deleteClicked(var postData)
    signal likeClicked()

    ColumnLayout {
        id: postContent
        width: parent.width
        spacing: 0

        Text {
            text: postData.text
            color: "white"
            font.pixelSize: 14
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.topMargin: 20
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.bottomMargin: postData.imageUrl ? 10 : 15
        }

        Item {
            id: imageContainer
            Layout.fillWidth: true
            Layout.preferredHeight: postData.imageUrl ? 180 : 0
            visible: postData.imageUrl
            clip: true

            Image {
                id: postImage
                source: postData.imageUrl
                width: parent.width
                height: parent.height
                fillMode: Image.PreserveAspectCrop
                anchors.centerIn: parent
                asynchronous: true
                cache: true

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle {
                        width: postImage.width
                        height: postImage.height
                        radius: 8
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    color: "#40000000"
                    visible: postImage.status === Image.Loading

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: true
                        width: 40
                        height: 40
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: imagePreview.show(postData.imageUrl)
            }
        }

        Item {
            Layout.preferredHeight: postData.imageUrl ? 0 : 15
            Layout.fillWidth: true
            visible: !postData.imageUrl
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#3A3A4E"
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.bottomMargin: 15
        }

        // Основная строка с лайками и датой
        RowLayout {
            spacing: 15
            Layout.bottomMargin: postData.isMine ? 0 : 15
            Layout.leftMargin: 15
            Layout.rightMargin: 15

            Rectangle {
                width: 60
                height: 24
                color: "transparent"

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onPressed: emojiHeart.scale = 0.9
                    onReleased: emojiHeart.scale = 1.0
                    onClicked: {
                        var newLiked = !postData.liked;
                        var newLikes = postData.likes + (newLiked ? 1 : -1);
                        postData.liked = newLiked;
                        postData.likes = newLikes;
                        emojiBounce.start();
                    }

                    Text {
                        id: emojiHeart
                        text: postData.liked ? "❤️" : "🤍"
                        font.pixelSize: 16
                        color: postData.liked ? "#FF5252" : "white"
                        anchors.centerIn: parent
                        renderType: Text.NativeRendering
                        layer.enabled: true
                        layer.effect: DropShadow {
                            radius: 2
                            samples: 4
                            color: "#80000000"
                            verticalOffset: 1
                        }
                    }

                    Text {
                        text: postData.likes
                        color: postData.liked ? "#FF5252" : "#B0B0D0"
                        font.pixelSize: 12
                        anchors.left: emojiHeart.right
                        anchors.leftMargin: 5
                        anchors.verticalCenter: emojiHeart.verticalCenter
                    }
                }

                SequentialAnimation {
                    id: emojiBounce
                    NumberAnimation {
                        target: emojiHeart
                        property: "scale"
                        from: 1.0
                        to: 1.3
                        duration: 100
                    }
                    NumberAnimation {
                        target: emojiHeart
                        property: "scale"
                        from: 1.3
                        to: 1.0
                        duration: 200
                        easing.type: Easing.OutBounce
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: postData.date
                color: "#707090"
                font.pixelSize: 12
            }
        }

        // Строка с кнопками (только для своих постов)
        Row {
            visible: postData.isMine
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 15
            Layout.bottomMargin: 10
            Layout.topMargin: 40
            spacing: 15

            // Кнопка редактирования
            Text {
                id: editButton
                text: "✏️"
                font.pixelSize: 14
                color: editMouseArea.containsMouse ? "#4FC3F7" : "#AAAAAA"
                Behavior on color { ColorAnimation { duration: 150 } }

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width * 1.8
                    height: width
                    radius: width / 2
                    color: "#4FC3F7"
                    opacity: editMouseArea.containsMouse ? 0.15 : 0
                    Behavior on opacity { NumberAnimation { duration: 200 } }
                }

                MouseArea {
                    id: editMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: editButton.scale = 1.1
                    onExited: editButton.scale = 1.0
                    onPressed: editButton.scale = 0.9
                    onReleased: editButton.scale = editMouseArea.containsMouse ? 1.1 : 1.0
                    onClicked: root.editClicked(root.postData)
                }

                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            // Кнопка удаления
            Text {
                id: deleteButton
                text: "🗑️"
                font.pixelSize: 14
                color: deleteMouseArea.containsMouse ? "#FF5555" : "#AAAAAA"
                Behavior on color { ColorAnimation { duration: 150 } }

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width * 1.8
                    height: width
                    radius: width / 2
                    color: "#FF5555"
                    opacity: deleteMouseArea.containsMouse ? 0.15 : 0
                    Behavior on opacity { NumberAnimation { duration: 200 } }
                }

                MouseArea {
                    id: deleteMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: deleteButton.scale = 1.1
                    onExited: deleteButton.scale = 1.0
                    onPressed: deleteButton.scale = 0.9
                    onReleased: deleteButton.scale = deleteMouseArea.containsMouse ? 1.1 : 1.0
                    onClicked: root.deleteClicked(root.postData)
                }

                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }
        }
    }

    ImagePreview {
        id: imagePreview
    }
}
