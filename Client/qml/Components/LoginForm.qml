import QtQuick
import QtQuick.Layouts

ColumnLayout {
    spacing: 15
    width: parent.width
    signal createAccountClicked
    signal loginSuccess

    // Поле для email
    CustomTextField {
        id: email
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        placeholderText: "your@bridge.world"
        placeholderColor: "#FFECD280"
        backgroundColor: "#301A3A60"
    }

    // Поле для пароля
    CustomTextField {
        id: password
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        placeholderText: "••••••••"
        echoMode: TextInput.Password
        placeholderColor: "#FFECD280"
        backgroundColor: "#301A3A60"
    }

    // Кнопка входа
    CustomButton {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        text: "Log In"
        onClicked: {
            if (!email.text || !password.text) {
                errorText.text = "Please fill all fields."
                errorText.opacity = 1
                errorMessageTimer.restart()
            } else {
                // Отправляем запрос на вход через NetworkManager
                networkManager.loginUser(email.text, password.text)
            }
        }
    }

    // Сообщение об ошибке
    Text {
        id: errorText
        text: ""
        color: "#FF4D4D"
        font.pixelSize: 14
        opacity: 0
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: parent.width * 0.8
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

    // Таймер для автоматического скрытия сообщения об ошибке
    Timer {
        id: errorMessageTimer
        interval: 3000 // 3 секунды
        onTriggered: {
            errorText.opacity = 0 // Скрываем сообщение
        }
    }

    // Ссылка на создание аккаунта
    TextLink {
        text: "Create Account"
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        onClicked: {
            createAccountClicked()
            if (stackView) {
                stackView.push(registerPage)
            }
        }
    }

    // Обработка сигналов от NetworkManager
    Connections {
        target: networkManager
        function onLoginSuccess() {
            // Уведомляем родительский компонент об успешном входе
            loginSuccess()
        }

        function onLoginFailed(errorMessage) {
            errorText.text = errorMessage
            errorText.opacity = 1
            errorMessageTimer.restart()
        }
    }
}
