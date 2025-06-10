import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


import Client.Components
import Client.Pages

Window {
    id: mainWindow
    width: 380
    height: 560
    visible: true
    color: "transparent"
    flags: Qt.Window | Qt.FramelessWindowHint

    MainBackground {
        anchors.fill: parent
        radius: 25
    }

    AppHeader {
        id: appHeader
        anchors.top: parent.top
        anchors.topMargin: 50 // Опускаем ниже
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
    }


    StackView {
        id: stackView
        anchors {
            top: appHeader.bottom // Начинаем ниже AppHeader
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 10 // Отступ от AppHeader
        }

        initialItem: loginPage


        pushEnter: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 300
                }
                NumberAnimation {
                    property: "y"
                    from: stackView.height
                    to: 0
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }
        }

        popExit: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 300
                }
                NumberAnimation {
                    property: "x"
                    from: 0
                    to: stackView.height
                    duration: 300
                    easing.type: Easing.InCubic
                }
            }
        }
    }

    Component {
        id: loginPage
        LoginPage {
        onLoginSuccess:
        {
            var profilePage = Qt.createComponent("ProfilePage.qml")
            var window = profilePage.createObject()
            mainWindow.hide()
            window.show()
        }
        }
    }

    Component {
        id: registerPage
        RegisterPage {}
    }

    DragHandler {
        target: null
        onActiveChanged: if (active) mainWindow.startSystemMove()
    }
}
