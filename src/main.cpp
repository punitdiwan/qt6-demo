#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include "logger.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "singleinstance.h"
#include "updater/updatemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("MaitretechSolution");
    app.setOrganizationDomain("maitretech.com");
    app.setApplicationName("HelloQt6");
    app.setApplicationVersion(APP_VERSION);

    // ── Log directory ───────────────────────────────────────────────────────
    // On Windows the installer creates %ProgramData%\MaitretechSolution\HelloQt6\
    // and removes it on uninstall. We write app.log there so install.log and
    // app.log live side-by-side and are cleaned up together.
    // On Linux we fall back to the standard writable data location.
#ifdef Q_OS_WIN
    const QString logDir = QDir(qEnvironmentVariable("ProgramData"))
                               .filePath("MaitretechSolution/HelloQt6");
#else
    const QString logDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif

    Logger::instance().setLogFile(logDir + "/app.log");
    Logger::write("App", QString("=== Started v%1 ===").arg(APP_VERSION));

    // ── Single instance guard ───────────────────────────────────────────────
    // Uses the UpgradeCode GUID as the key so it is unique to this product.
    SingleInstanceGuard guard("HelloQt6-{3F7B9E2A-C4D1-4E8F-A7B3-2C5D9E1F0A4B}");
    if (!guard.isPrimary()) {
        Logger::write("App", "Another instance is running — exiting.");
        return 0;
    }

    // ── Update-check URL ────────────────────────────────────────────────────
    // Written by the installer:  HKLM\Software\MaitretechSolution\HelloQt6\UpdateUrl
#ifdef Q_OS_WIN
    QSettings sysReg(
        R"(HKEY_LOCAL_MACHINE\Software\MaitretechSolution\HelloQt6)",
        QSettings::NativeFormat);
    const QString updateUrl = sysReg.value(
        "UpdateUrl",
        "https://github.com/your-org/HelloQt6/releases/latest/download/version.json"
    ).toString();
#else
    QSettings cfg;
    const QString updateUrl = cfg.value(
        "updateUrl",
        "https://github.com/your-org/HelloQt6/releases/latest/download/version.json"
    ).toString();
#endif

    Logger::write("App", "Update URL: " + updateUrl);
    UpdateManager::instance().setCheckUrl(updateUrl);

    // ── Login gate ──────────────────────────────────────────────────────────
    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        Logger::write("App", "Login cancelled — exiting.");
        return 0;
    }

    MainWindow window;
    window.show();

    // If a second instance starts while we are running, bring our window up.
    QObject::connect(&guard, &SingleInstanceGuard::secondaryInstanceStarted,
                     &window, &MainWindow::bringToFront);

    UpdateManager::instance().checkForUpdates();

    const int ret = app.exec();
    Logger::write("App", QString("=== Exited (code %1) ===").arg(ret));
    return ret;
}
