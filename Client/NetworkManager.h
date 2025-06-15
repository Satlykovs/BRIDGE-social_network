#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QDateTime>

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

    // Основной метод для JSON запросов
    Q_INVOKABLE  void sendRequest(const QUrl& url, HttpMethod method,
                                 QObject* receiver, const char* replySlot,
                                 const QJsonObject& body = {},
                                 bool needsAuth = false,
                                 const QVariant& userData = QVariant());

    // Методы аутентификации
    Q_INVOKABLE void registerUser(const QString& email, const QString& password);
    Q_INVOKABLE void loginUser(const QString& email, const QString& password);
    void checkSavedTokens();
    void refreshTokens();
    Q_INVOKABLE void logout();

signals:
    void registrationSuccess();
    void registrationFailed(const QString& error);
    void loginSuccess();
    void loginFailed(const QString& error);
    void tokenRefreshSuccess();
    void tokenRefreshNeeded();
    void tokenRefreshFailed();
    void authRequired();
    void loggedOut();

private slots:
    // Исправлено: добавлен параметр QNetworkReply*
    void onRegistrationFinished(QNetworkReply* reply);
    void onLoginFinished(QNetworkReply* reply);
    void onRefreshFinished(QNetworkReply* reply);

private:
    struct TokenData {
        QString authToken;
        QString refreshToken;
        QDateTime authExpiry;
        QDateTime refreshExpiry;


        bool isAuthValid() const {
            qDebug() << authExpiry <<  authExpiry.toUTC() <<  QDateTime::currentDateTime() << QDateTime::currentDateTimeUtc();
            return !authToken.isEmpty() && authExpiry.isValid() && authExpiry > QDateTime::currentDateTime();
        }

        bool isRefreshValid() const {
            return !refreshToken.isEmpty() && refreshExpiry.isValid() && refreshExpiry > QDateTime::currentDateTimeUtc();
        }

        void clear() {
            authToken.clear();
            refreshToken.clear();
            authExpiry = QDateTime();
            refreshExpiry = QDateTime();
        }
    };

    void saveTokens(const QString& authToken, const QString& refreshToken,
                    const QDateTime& authExpiry, const QDateTime& refreshExpiry);
    void clearTokens();
    QDateTime parseDateTime(const QString& dateTimeStr);

    QNetworkAccessManager* networkManager_;
    TokenData m_tokenData;
    bool m_isInitialAuth;

    void sendRequestInternal(const QUrl& url, HttpMethod method,
                             QObject* receiver, const char* replySlot,
                             const QJsonObject& body,
                             bool needsAuth,
                             const QVariant& userData,
                             int retryCount);
};

#endif // NETWORKMANAGER_H
