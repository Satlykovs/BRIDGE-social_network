#ifndef PROFILELOADER_H
#define PROFILELOADER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include "NetworkManager.h"

class ProfileLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString firstName READ firstName NOTIFY dataChanged)
    Q_PROPERTY(QString lastName READ lastName NOTIFY dataChanged)
    Q_PROPERTY(QString username READ username NOTIFY dataChanged)
    Q_PROPERTY(QUrl avatarUrl READ avatarUrl NOTIFY dataChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit ProfileLoader(QObject *parent = nullptr);

    // Сеттер для инъекции зависимости
    Q_INVOKABLE void setNetworkManager(NetworkManager* manager);

    Q_INVOKABLE void loadProfile();

    QString firstName() const { return m_firstName; }
    QString lastName() const { return m_lastName; }
    QString username() const { return m_username; }
    QUrl avatarUrl() const { return m_avatarUrl; }
    bool isLoading() const { return m_isLoading; }

signals:
    void dataChanged();
    void isLoadingChanged();
    void loadError(const QString& message);

private slots:
    void onProfileReply(QNetworkReply* reply);

private:
    NetworkManager* m_networkManager = nullptr;
    QString m_firstName;
    QString m_lastName;
    QString m_username;
    QUrl m_avatarUrl;
    bool m_isLoading = false;
};

#endif // PROFILELOADER_H
