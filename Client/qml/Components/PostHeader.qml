import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    signal backClicked()
    height: 60
    color: "#2A2A3E"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        Rectangle {
            width: 40
            height: 40
            color: "transparent"

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: backClicked()

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
