import QtQuick
import QtQuick.Layouts

ColumnLayout {
    spacing: 15

    // Поле Email
    CustomTextField {
        id: emailField
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        placeholderText: "your@email.com"
        placeholderColor: "#FFECD280"
        backgroundColor: "#301A3A60"
    }

    // Поле Password
    CustomTextField {
        id: passwordField
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        placeholderText: "••••••••"
        echoMode: TextInput.Password
        placeholderColor: "#FFECD280"
        backgroundColor: "#301A3A60"
    }

    // Поле Confirm Password
    CustomTextField {
        id: confirmPasswordField
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        placeholderText: "confirm password"
        echoMode: TextInput.Password
        placeholderColor: "#FFECD280"
        backgroundColor: "#301A3A60"
    }

    // Сообщение об ошибке/успехе
    Text {
        id: statusText
        width: Math.min(implicitWidth, parent.width - 40) // Ограничиваем ширину
        text: ""
        color: "#FF4D4D"
        font.pixelSize: 14
        opacity: 0
        wrapMode: Text.Wrap // Включаем перенос текста
        maximumLineCount: 3 // Максимум 3 строки
        horizontalAlignment: Text.AlignHCenter
        Layout.maximumWidth: parent.width - 40 // Дополнительное ограничение для Layout
        Layout.alignment: Qt.AlignHCenter
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

    // Таймер для автоматического скрытия сообщения
    Timer {
        id: statusMessageTimer
        interval: 3000 // 3 секунды
        onTriggered: {
            statusText.opacity = 0 // Скрываем сообщение
        }
    }

    Timer {
        id: returnDelayTimer
        interval: 1000 // 1 секунда
        onTriggered: {
            if (stackView) {
                stackView.pop() // Возвращаемся на предыдущую страницу
            }
        }
    }

    // Кнопка Sign In
    CustomButton {
        Layout.alignment: Qt.AlignHCenter
        text: "Sign in"
        onClicked: {
            if (!emailField.text || !passwordField.text || !confirmPasswordField.text) {
                // Если поля не заполнены
                statusText.text = "Please fill all fields."
                statusText.color = "#FF4D4D" // Красный цвет для ошибки
                statusText.opacity = 1
                statusMessageTimer.restart()
            } else if (passwordField.text !== confirmPasswordField.text) {
                // Если пароли не совпадают
                statusText.text = "Passwords do not match."
                statusText.color = "#FF4D4D"
                statusText.opacity = 1
                statusMessageTimer.restart()
            } else {
                // Отправляем запрос на регистрацию
                networkManager.registerUser(emailField.text, passwordField.text)
            }
        }
    }

    // Обработка сигналов от NetworkManager
    Connections {
        target: networkManager
        function onRegistrationSuccess() {
            statusText.text = "Registration successful!"
            statusText.color = "#4CAF50" // Зеленый цвет для успеха
            statusText.opacity = 1
            statusMessageTimer.restart()

            // Запускаем таймер для задержки перед возвратом
            returnDelayTimer.start()
        }

        function onRegistrationFailed(errorMessage) {
            statusText.text = errorMessage
            statusText.color = "#FF4D4D" // Красный цвет для ошибки
            statusText.opacity = 1
            statusMessageTimer.restart()
        }
    }

    // Кнопка возврата
    TextLink {
        text: "Back to Login"
        Layout.alignment: Qt.AlignHCenter
        onClicked: if (stackView) stackView.pop()
    }
}
