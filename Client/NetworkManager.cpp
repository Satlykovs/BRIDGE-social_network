#include "NetworkManager.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSettings>

NetworkManager::NetworkManager(QObject* parent) : QObject(parent), networkManager_(new QNetworkAccessManager(this)) {}

void NetworkManager::sendRequest(const QUrl &url, NetworkManager::HttpMethod method, const QObject *receiver, const char *replySlot, const QJsonObject &body, bool needsAuth)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (needsAuth) {
        QString token = getAuthToken();
        if (token.isEmpty()) {
            qWarning() << "Request to" << url << "requires authentication, but no token is available.";
            // В будущем здесь можно создать "фейковый" ответ с ошибкой и передать его в слот
            return;
        }
        request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    }

    QNetworkReply* reply = nullptr;
    QByteArray requestBody = QJsonDocument(body).toJson();

    switch (method) {
    case HttpMethod::Get:
        reply = networkManager_->get(request);
        break;
    case HttpMethod::Post:
        reply = networkManager_->post(request, requestBody);
        break;
    case HttpMethod::Put:
        reply = networkManager_->put(request, requestBody);
        break;
    case HttpMethod::Patch:
        reply = networkManager_->sendCustomRequest(request, "PATCH", requestBody);
        break;
    case HttpMethod::Delete:
        reply = networkManager_->deleteResource(request);
        break;
    }

    if (reply) {
        connect(reply, &QNetworkReply::finished, receiver, [receiver, reply, replySlot](){
            QMetaObject::invokeMethod(const_cast<QObject*>(receiver), replySlot, Q_ARG(QNetworkReply*, reply));
        });
    }
}

QString NetworkManager::getAuthToken()
{
    if (m_authToken.isEmpty()) {
        QSettings settings;
        m_authToken = settings.value("authToken").toString();
    }
    return m_authToken;
}

void NetworkManager::registerUser(const QString& email, const QString& password)
{
    QUrl url("http://localhost:8080/auth/register");
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    sendRequest(url, HttpMethod::Post, this, "onRegistrationFinished", json, false);
}

void NetworkManager::onRegistrationFinished(QNetworkReply* reply)
{
    qDebug() << "Registration request finished. Error: " << reply->error();

    if (reply->error() == QNetworkReply::NoError) {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            emit registrationSuccess();
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QString errorMsg = doc.object().value("error").toString("Server error");
            emit registrationFailed(errorMsg);
        }
    } else {
        emit registrationFailed(reply->errorString());
    }
    reply->deleteLater();
}

void NetworkManager::loginUser(const QString& email, const QString& password)
{
    QUrl url("http://localhost:8080/auth/login");
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    sendRequest(url, HttpMethod::Post, this, "onLoginFinished", json, false);
}

void NetworkManager::onLoginFinished(QNetworkReply* reply)
{
    qDebug() << "Login request finished. Error: " << reply->error();

    if (reply->error() == QNetworkReply::NoError) {
        QString authHeader = reply->rawHeader("Authorization");
        QString refreshToken = reply->rawHeader("X-Refresh-Token");

        if (!authHeader.isEmpty() && authHeader.startsWith("Bearer ")) {
            QString authToken = authHeader.mid(7);

            if (!authToken.isEmpty() && !refreshToken.isEmpty()) {
                saveTokens(authToken, refreshToken);
                m_authToken = authToken; // Сохраняем токен в переменную класса
                emit loginSuccess();
                reply->deleteLater();
                return;
            }
        }
        emit loginFailed("Invalid response from server");
    } else {
        emit loginFailed(reply->errorString());
    }

    reply->deleteLater();
}

void NetworkManager::saveTokens(const QString& authToken, const QString& refreshToken)
{
    QSettings settings;
    settings.setValue("authToken", authToken);
    settings.setValue("refreshToken", refreshToken);
}

void NetworkManager::checkSavedTokens()
{
    QSettings settings;
    QString authToken = settings.value("authToken").toString();
    QString refreshToken = settings.value("refreshToken").toString();

    if (!authToken.isEmpty() && !refreshToken.isEmpty()) {
        qDebug() << "Tokens found: User is logged in.";
        emit loginSuccess();
    } else {
        qDebug() << "No tokens found: User needs to log in.";
    }
}
