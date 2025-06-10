#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum class HttpMethod {
        Get,
        Post,
        Put,
        Patch,
        Delete
    };
    Q_ENUM(HttpMethod)

    explicit NetworkManager(QObject* parent = nullptr);

    // Универсальный публичный метод для отправки запросов
    Q_INVOKABLE void sendRequest(const QUrl& url,
                                 HttpMethod method,
                                 const QObject* receiver,
                                 const char* replySlot,
                                 const QJsonObject& body = QJsonObject(),
                                 bool needsAuth = true);

    // Методы для аутентификации
    void registerUser(const QString& email, const QString& password);
    void loginUser(const QString& email, const QString& password);
    void checkSavedTokens();

signals:
    void registrationSuccess();
    void registrationFailed(const QString& error);
    void loginSuccess();
    void loginFailed(const QString& error);

private slots:
    void onRegistrationFinished(QNetworkReply* reply);
    void onLoginFinished(QNetworkReply* reply);

private:
    void saveTokens(const QString& authToken, const QString& refreshToken);
    QString getAuthToken();

    QNetworkAccessManager* networkManager_;
    QString m_authToken;
};

#endif // NETWORKMANAGER_H
