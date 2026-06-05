#include <QApplication>
#include <QSettings>
#include "mainwindow.h"
#include "updater/updatemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("MaitretechSolution");
    app.setOrganizationDomain("maitretech.com");
    app.setApplicationName("HelloQt6");
    app.setApplicationVersion(APP_VERSION);

    // Read the update-check URL that the installer writes to the system registry
    // (HKLM\Software\MaitretechSolution\HelloQt6 → UpdateUrl).
    // Falls back to the default endpoint if the key is absent (dev builds, Linux).
#ifdef Q_OS_WIN
    QSettings sysReg(
        R"(HKEY_LOCAL_MACHINE\Software\MaitretechSolution\HelloQt6)",
        QSettings::NativeFormat);
    const QString updateUrl = sysReg.value(
        "UpdateUrl",
        "https://maitretech.com/releases/HelloQt6/version.json").toString();
#else
    QSettings cfg;
    const QString updateUrl = cfg.value(
        "updateUrl",
        "https://maitretech.com/releases/HelloQt6/version.json").toString();
#endif

    UpdateManager::instance().setCheckUrl(updateUrl);

    MainWindow window;
    window.show();

    // Kick off a background update check after the window is visible.
    // Results arrive asynchronously via UpdateManager signals wired in MainWindow.
    UpdateManager::instance().checkForUpdates();

    return app.exec();
}
