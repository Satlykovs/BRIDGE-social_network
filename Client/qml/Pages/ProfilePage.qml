import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects


import Client.Components
import Client.Pages
import Client

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

    property var userData: ({
        "firstName": "Arnak",
        "lastName": "Hovsepyan",
        "username": "ARHov",
        "avatarUrl": "qrc:/images/avatar.png",
        "email": "hovsep.ar@bridge.world"
    })

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

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.topMargin: 60
        spacing: 20

        Avatar {
            source: userData.avatarUrl
            Layout.alignment: Qt.AlignHCenter
        }

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

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: "Мои посты"
            onClicked: postsPage.visible = true

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
        }
    }

    PostPage {
        id: postsPage
        visible: false
    }

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
