import Client.Components


import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


    Item {
        id: parent
        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width * 0.8
            spacing: 30

            RegisterForm {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter; layoutDirection: Qt.LeftToRight
            }
        }
    }
