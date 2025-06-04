#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent = nullptr);
    Q_INVOKABLE void registerUser(const QString& email, const QString& password);
    Q_INVOKABLE void loginUser(const QString& email, const QString& password);

    void checkSavedTokens();

signals:
    void registrationSuccess();
    void registrationFailed(const QString& errorMessage);

    void loginSuccess();
    void loginFailed(const QString& errorMessage);

private slots:
    void onRegistrationFinished(QNetworkReply* reply);
    void onLoginFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* networkManager_;

    void saveTokens(const QString& authToken, const QString& refreshToken);

};

#endif // NETWORKMANAGER_H
