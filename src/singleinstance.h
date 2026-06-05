#pragma once

#include <QLocalServer>
#include <QObject>
#include <QString>

// Ensures only one instance of the app runs at a time.
//
// How it works:
//   Primary instance   → creates a QLocalServer listening on a named key.
//   Secondary instance → connects to that server, sends "raise", then exits.
//   Primary instance   → receives "raise", emits secondaryInstanceStarted(),
//                        which main.cpp uses to restore/focus the window.
//
// Usage in main.cpp:
//   SingleInstanceGuard guard("HelloQt6-{your-upgrade-guid}");
//   if (!guard.isPrimary()) return 0;
//   ... create window ...
//   QObject::connect(&guard, &SingleInstanceGuard::secondaryInstanceStarted,
//                    &window, &MainWindow::bringToFront);

class SingleInstanceGuard : public QObject {
    Q_OBJECT
public:
    explicit SingleInstanceGuard(const QString &key, QObject *parent = nullptr);
    ~SingleInstanceGuard() override;

    bool isPrimary() const;

signals:
    // Emitted on the primary instance when a second launch is attempted.
    void secondaryInstanceStarted();

private:
    void onNewConnection();

    QString       m_key;
    bool          m_primary = false;
    QLocalServer *m_server  = nullptr;
};
