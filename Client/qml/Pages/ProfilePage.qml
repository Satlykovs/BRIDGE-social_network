import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes

import Client.Components

Window {
    id: window
    width: 380
    height: 680
    minimumWidth: 360
    minimumHeight: 600
    visible: true
    title: "Profile"
    color: "#1A1A2E"
    flags: Qt.Window | Qt.FramelessWindowHint

    property real contentWidth: Math.min(380, window.width * 0.9)

    // Данные пользователя
    property var userData: ({
        "firstName": "Arnak",
        "lastName": "Hovsepyan",
        "username": "ARHov",
        "avatarUrl": "qrc:/images/avatar.png",
        "email": "hovsep.ar@bridge.world"
    })

    // Модель постов
    ListModel {
        id: postsModel

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

    // Фон приложения
    Image {
        source: userData.avatarUrl
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        layer.enabled: true
        layer.effect: FastBlur { radius: 32 }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#AA000000" }
            GradientStop { position: 1.0; color: "#661A1A2E" }
        }
    }

    // Главная страница профиля
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.topMargin: 60
        spacing: 20

        // Аватар
        Item {
            width: 160
            height: 160
            Layout.alignment: Qt.AlignHCenter

            layer.enabled: true
            layer.effect: DropShadow {
                radius: 16
                samples: 24
                color: "#80000000"
                verticalOffset: 4
            }

            OpacityMask {
                anchors.fill: parent
                source: Image {
                    source: userData.avatarUrl
                    fillMode: Image.PreserveAspectCrop
                }
                maskSource: Rectangle {
                    width: 160
                    height: 160
                    radius: 80
                }
            }
        }

        // Имя и username
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 5

            Text {
                text: userData.firstName + " " + userData.lastName
                color: "white"
                font {
                    family: "Montserrat"
                    pixelSize: 26
                    weight: Font.Bold
                }
            }
            Text {
                text: "@" + userData.username
                color: "#B0B0D0"
                font { pixelSize: 17; italic: true }
            }
        }

        // Кнопка перехода к постам
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: "Мои посты"

            background: Rectangle {
                radius: 18
                color: Qt.rgba(98/255, 0/255, 238/255, 0.7)
                border.color: "#6200EE"
                border.width: 2
            }

            contentItem: Text {
                text: parent.text
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                font {
                    pixelSize: 14
                    bold: true
                }
            }

            onClicked: postPage.visible = true
        }
    }

    // Страница с постами
    Rectangle {
        id: postPage
        anchors.fill: parent
        visible: false
        color: "#1E1E2E"

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // Шапка с кнопкой назад
            Rectangle {
                Layout.fillWidth: true
                height: 60
                color: "#2A2A3E"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10

                    // Кнопка "Назад"
                    Rectangle {
                        width: 40
                        height: 40
                        color: "transparent"

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: postPage.visible = false

                            Canvas {
                                anchors.centerIn: parent
                                width: 24
                                height: 24
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.strokeStyle = "white"
                                    ctx.lineWidth = 2
                                    ctx.lineCap = "round"
                                    ctx.beginPath()
                                    ctx.moveTo(16, 8)
                                    ctx.lineTo(8, 12)
                                    ctx.lineTo(16, 16)
                                    ctx.stroke()
                                }
                            }
                        }
                    }

                    Text {
                        text: "Мои публикации"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            // Список постов
            ListView {
                id: postsListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: postsModel
                spacing: 20
                boundsBehavior: Flickable.StopAtBounds

                delegate: Rectangle {
                    width: Math.min(340, window.contentWidth - 40)
                    height: postContent.implicitHeight + 50
                    radius: 12
                    color: "#2A2A3E"
                    border.color: "#3A3A4E"
                    border.width: 1
                    anchors.horizontalCenter: parent.horizontalCenter

                    ColumnLayout {
                        id: postContent
                        width: parent.width
                        spacing: 0

                        // Текст поста
                        Text {
                            text: model.text
                            color: "white"
                            font.pixelSize: 14
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                            Layout.topMargin: 20
                            Layout.leftMargin: 15
                            Layout.rightMargin: 15
                            Layout.bottomMargin: model.imageUrl ? 10 : 15
                        }

                        // Контейнер для изображения
                        Item {
                            id: imageContainer
                            Layout.fillWidth: true
                            Layout.preferredHeight: model.imageUrl ? 180 : 0
                            visible: model.imageUrl
                            clip: true

                            Image {
                                id: postImage
                                source: model.imageUrl
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

                                // Индикатор загрузки
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
                                onClicked: imagePreview.show(model.imageUrl)
                            }
                        }

                        // Разделитель с отступом
                        Item {
                            Layout.preferredHeight: model.imageUrl ? 0 : 15
                            Layout.fillWidth: true
                            visible: !model.imageUrl
                        }

                        // Линия-разделитель
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#3A3A4E"
                            Layout.leftMargin: 15
                            Layout.rightMargin: 15
                            Layout.bottomMargin: 15
                        }

                        // Нижняя панель
                        RowLayout {
                            spacing: 15
                            Layout.bottomMargin: 20
                            Layout.leftMargin: 15
                            Layout.rightMargin: 15

                            // Кнопка лайка
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
                                        var newLiked = !model.liked;
                                        var newLikes = model.likes + (newLiked ? 1 : -1);
                                        postsModel.setProperty(index, "liked", newLiked);
                                        postsModel.setProperty(index, "likes", newLikes);
                                        emojiBounce.start();
                                    }

                                    Text {
                                        id: emojiHeart
                                        text: model.liked ? "❤️" : "🤍"
                                        font.pixelSize: 16
                                        color: model.liked ? "#FF5252" : "white"
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
                                        text: model.likes
                                        color: model.liked ? "#FF5252" : "#B0B0D0"
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
                                text: model.date
                                color: "#707090"
                                font.pixelSize: 12
                            }
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent
                    width: parent.width
                    visible: postsModel.count === 0
                    text: "Публикаций пока нет"
                    color: "#707090"
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 16
                    padding: 30
                }
            }
        }

        OpacityAnimator {
            target: postPage
            from: 0
            to: 1
            duration: 200
            running: postPage.visible
        }
    }

    // Превью изображения
    Popup {
        id: imagePreview
        width: window.width
        height: window.height
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property alias imageSource: fullSizeImage.source

        function show(source) {
            imageSource = source;
            open();
        }

        Rectangle {
            anchors.fill: parent
            color: "#CC000000"

            Item {
                width: Math.min(parent.width * 0.9, parent.height * 0.9)
                height: width
                anchors.centerIn: parent

                Image {
                    id: fullSizeImage
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    asynchronous: true

                    MouseArea {
                        anchors.fill: parent
                        onClicked: imagePreview.close()
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: fullSizeImage.status === Image.Loading
                        width: 60
                        height: 60
                    }
                }

                RoundButton {
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: -20
                    icon.source: "qrc:/images/close.png"
                    icon.color: "white"
                    flat: true
                    onClicked: imagePreview.close()
                }
            }
        }
    }

    // Управление окном
    WindowControls {
        window: window
        anchors.top: parent.top
        anchors.right: parent.right
    }

    DragHandler {
        target: null
        onActiveChanged: if (active) window.startSystemMove()
    }
}
