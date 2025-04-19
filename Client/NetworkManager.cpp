#include "NetworkManager.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSettings>


NetworkManager::NetworkManager(QObject* parent) : QObject(parent), networkManager_(new QNetworkAccessManager(this)) {}


void NetworkManager::registerUser(const QString& email, const QString& password)
{
    QUrl url("http://localhost:8080/auth/register");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QNetworkReply* reply = networkManager_->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onRegistrationFinished(reply);
    });
}

void NetworkManager::onRegistrationFinished(QNetworkReply* reply)
{
    qDebug() << "Request finished. Error: " << reply->error();
    qDebug() << "Response: " << reply->readAll();

    if (reply->error() == QNetworkReply::NoError)
    {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 200)
        {
            emit registrationSuccess();
        }
        else
        {
            emit registrationFailed("Server error");
        }
    }
    else
    {
        emit registrationFailed(reply->errorString());
    }

    reply->deleteLater();
}


void NetworkManager::loginUser(const QString& email, const QString& password)
{
    QUrl url("http://localhost:8080/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QNetworkReply* reply = networkManager_->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        onLoginFinished(reply);
    });
}

void NetworkManager::onLoginFinished(QNetworkReply* reply)
{

    qDebug() << "Request finished. Error: " << reply->error();

    if (reply->error() == QNetworkReply::NoError)
    {
        QString authHeader = reply->rawHeader("Authorization");
        QString refreshToken = reply->rawHeader("X-Refresh-Token");

        qDebug() << "Auth header: " << authHeader;
        qDebug()  << "Refresh token: " << refreshToken;

        if (!authHeader.isEmpty()  && authHeader.startsWith("Bearer "))
        {
            QString authToken = authHeader.mid(7);

            if (!authToken.isEmpty() && !refreshToken.isEmpty())
            {
                saveTokens(authToken, refreshToken);

                emit loginSuccess();
            }

            emit loginFailed("Invalid response from server");
        }
    }
    else
    {
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
