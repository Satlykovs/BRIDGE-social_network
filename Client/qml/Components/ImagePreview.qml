import QtQuick
import QtQuick.Controls

Popup {
    id: imagePreview
    width: parent.width
    height: parent.height
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property alias source: fullSizeImage.source

    function show(source) {
        imagePreview.source = source;
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
