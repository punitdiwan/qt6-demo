#include "singleinstance.h"
#include "logger.h"

#include <QLocalSocket>

SingleInstanceGuard::SingleInstanceGuard(const QString &key, QObject *parent)
    : QObject(parent), m_key(key)
{
    // Try to reach an already-running instance.
    QLocalSocket probe;
    probe.connectToServer(m_key);

    if (probe.waitForConnected(500)) {
        // Primary instance is alive — tell it to raise its window then exit.
        probe.write("raise");
        probe.waitForBytesWritten(500);
        probe.disconnectFromServer();
        Logger::write("SingleInstance", "Secondary instance — signalled primary to raise.");
        m_primary = false;
        return;
    }

    // No response — we are the first instance.
    // Remove any stale socket left behind by a previous crash before listening.
    QLocalServer::removeServer(m_key);

    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
    m_server->listen(m_key);

    connect(m_server, &QLocalServer::newConnection,
            this,     &SingleInstanceGuard::onNewConnection);

    Logger::write("SingleInstance", "Primary instance started.");
    m_primary = true;
}

SingleInstanceGuard::~SingleInstanceGuard()
{
    if (m_server)
        QLocalServer::removeServer(m_key);
}

bool SingleInstanceGuard::isPrimary() const { return m_primary; }

void SingleInstanceGuard::onNewConnection()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QLocalSocket::readyRead, this, [this, socket] {
        const QByteArray msg = socket->readAll();
        Logger::write("SingleInstance", "Message from secondary: " + QString(msg));
        if (msg.trimmed() == "raise")
            emit secondaryInstanceStarted();
        socket->deleteLater();
    });
    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}
