import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Popup {
    id: root
    modal: true
    dim: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    width: Math.min(500, parent.width * 0.95)
    height: Math.min(550, parent.height * 0.9)
    x: (parent.width - width) / 2
    y: Math.max(10, (parent.height - height) / 2)
    padding: 15
    topInset: 0
    bottomInset: 0

    property int postId: -1
    property bool isNewPost: false
    property string postText: ""
    property string imageUrl: ""
    signal saveRequested(int postId, string newText, string newImageUrl)

    FileDialog {
        id: fileDialog
        title: "Выберите изображение"
        nameFilters: ["Изображения (*.png *.jpg *.jpeg)"]
        onAccepted: {
            root.imageUrl = selectedFile
        }
    }

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
            text: "Редактирование поста"
            color: "white"
            font.bold: true
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
        }

        // Основная область с прокруткой
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 150
            clip: true

            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            ScrollBar.vertical.interactive: true

            TextArea {
                id: textArea
                text: root.postText
                placeholderText: "Текст поста..."
                color: "white"
                font.pixelSize: 14
                wrapMode: TextArea.Wrap
                width: parent.width
                background: Rectangle {
                    color: "#3A3A4E"
                    radius: 8
                    border.color: textArea.text.trim() === "" ? "#FF5555" : "transparent"
                    border.width: textArea.text.trim() === "" ? 1 : 0
                }

                onTextChanged: {
                    errorLabel.visible = false
                }
            }
        }

        Label {
            id: errorLabel
            Layout.fillWidth: true
            visible: false
            text: "Текст поста не может быть пустым!"
            color: "#FF5555"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
        }

        // Превью изображения
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: "#3A3A4E"
            radius: 8
            visible: root.imageUrl || imagePreview.visible

            Image {
                id: imagePreview
                anchors.fill: parent
                anchors.margins: 5
                source: root.imageUrl
                fillMode: Image.PreserveAspectFit
                visible: source && status === Image.Ready
            }

            Label {
                anchors.centerIn: parent
                text: "Изображение отсутствует"
                color: "#707090"
                visible: !imagePreview.visible && root.imageUrl === ""
            }

            Label {
                anchors.centerIn: parent
                text: "Загрузка изображения..."
                color: "#707090"
                visible: root.imageUrl && imagePreview.status !== Image.Ready
            }
        }

        // Кнопки управления изображением
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "Загрузить изображение"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "#4FC3F7"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: fileDialog.open()
            }

            Button {
                text: "Удалить изображение"
                Layout.fillWidth: true
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
                onClicked: root.imageUrl = ""
                enabled: root.imageUrl !== ""
            }
        }

        // Кнопки сохранения/отмены
        RowLayout {
            Layout.fillWidth: true
            spacing: 15

            Button {
                text: "Отмена"
                Layout.fillWidth: true
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
                id: saveButton
                text: "Сохранить"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "#4FC3F7"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    // Проверяем, что текст не пустой
                    if (textArea.text.trim() === "") {
                        // Показываем ошибку и подсвечиваем поле
                        errorLabel.visible = true
                        textArea.forceActiveFocus()
                        return
                    }

                    // Если проверка пройдена, сохраняем
                    root.saveRequested(root.postId, textArea.text.trim(), root.imageUrl)
                    root.close()
                }
            }
        }
    }
}
