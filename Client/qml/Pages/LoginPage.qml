import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


import Client.Components

Item {
    id: parent
    signal loginSuccess

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, 400)
        spacing: 30

        LoginForm {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            onLoginSuccess: parent.loginSuccess()
        }
    }
}

