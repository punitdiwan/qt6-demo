#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class UpdateManager : public QObject {
    Q_OBJECT
public:
    static UpdateManager &instance();

    void    setCheckUrl(const QString &url);
    QString checkUrl() const;

    // Non-blocking. Results arrive via signals below.
    void checkForUpdates();

    // Call after receiving updateAvailable(). Downloads MSI to temp dir,
    // then emits readyToInstall(path) when done.
    void downloadUpdate(const QString &msiUrl);

signals:
    void updateAvailable(const QString &version,
                         const QString &msiUrl,
                         const QString &releaseNotes);
    void noUpdateAvailable();
    void checkError(const QString &message);
    void downloadProgress(qint64 received, qint64 total);
    void readyToInstall(const QString &localPath);

private:
    explicit UpdateManager(QObject *parent = nullptr);
    void handleVersionReply(QNetworkReply *reply);
    void handleDownloadReply(QNetworkReply *reply);

    QNetworkAccessManager *m_nam;
    QString                m_checkUrl;
};
