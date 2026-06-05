#include "updatemanager.h"
#include "../logger.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QVersionNumber>

#ifndef APP_VERSION
#  define APP_VERSION "0.0.0"
#endif

UpdateManager &UpdateManager::instance()
{
    static UpdateManager inst;
    return inst;
}

UpdateManager::UpdateManager(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{}

void UpdateManager::setCheckUrl(const QString &url) { m_checkUrl = url; }
QString UpdateManager::checkUrl() const             { return m_checkUrl; }

void UpdateManager::checkForUpdates()
{
    if (m_checkUrl.isEmpty()) {
        Logger::write("Updater", "No update URL configured — skipping check.");
        return;
    }

    Logger::write("Updater", "Checking for updates at: " + m_checkUrl);
    auto *reply = m_nam->get(QNetworkRequest(QUrl(m_checkUrl)));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        handleVersionReply(reply);
        reply->deleteLater();
    });
}

void UpdateManager::handleVersionReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        Logger::write("Updater", "Version check failed: " + reply->errorString());
        emit checkError(reply->errorString());
        return;
    }

    const QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
    const QString latest = obj.value("version").toString();
    const QString msiUrl = obj.value("msi_url").toString();
    const QString notes  = obj.value("release_notes").toString();

    if (latest.isEmpty() || msiUrl.isEmpty()) {
        Logger::write("Updater", "Invalid version manifest received.");
        emit checkError("Invalid version manifest received from server.");
        return;
    }

    const auto current = QVersionNumber::fromString(QString::fromLatin1(APP_VERSION));
    const auto remote  = QVersionNumber::fromString(latest);

    Logger::write("Updater",
        QString("Current: %1 | Remote: %2").arg(APP_VERSION, latest));

    if (remote > current) {
        Logger::write("Updater", "Update available: " + latest);
        emit updateAvailable(latest, msiUrl, notes);
    } else {
        Logger::write("Updater", "Already up to date.");
        emit noUpdateAvailable();
    }
}

void UpdateManager::downloadUpdate(const QString &msiUrl)
{
    Logger::write("Updater", "Downloading update from: " + msiUrl);
    auto *reply = m_nam->get(QNetworkRequest(QUrl(msiUrl)));

    connect(reply, &QNetworkReply::downloadProgress,
            this,  &UpdateManager::downloadProgress);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        handleDownloadReply(reply);
        reply->deleteLater();
    });
}

void UpdateManager::handleDownloadReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        Logger::write("Updater", "Download failed: " + reply->errorString());
        emit checkError(reply->errorString());
        return;
    }

    const QString dir  = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString path = dir + "/HelloQt6-update.msi";

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        Logger::write("Updater", "Cannot write update file: " + path);
        emit checkError("Cannot write update file to: " + path);
        return;
    }
    f.write(reply->readAll());
    f.close();

    Logger::write("Updater", "Update downloaded to: " + path);
    emit readyToInstall(path);
}
