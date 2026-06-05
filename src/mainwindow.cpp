#include "mainwindow.h"
#include "updater/updatemanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hello Qt6");
    setMinimumSize(320, 200);

    auto *central = new QWidget(this);
    auto *layout  = new QVBoxLayout(central);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    m_label = new QLabel("Hello, Qt6!", this);
    m_label->setAlignment(Qt::AlignCenter);
    QFont font = m_label->font();
    font.setPointSize(18);
    m_label->setFont(font);

    m_button = new QPushButton("Click me", this);
    m_button->setFixedWidth(120);

    layout->addWidget(m_label);
    layout->addWidget(m_button, 0, Qt::AlignCenter);

    setCentralWidget(central);

    connect(m_button, &QPushButton::clicked,
            this,     &MainWindow::onButtonClicked);

    // Wire auto-update signals from UpdateManager (check is fired in main.cpp)
    connect(&UpdateManager::instance(), &UpdateManager::updateAvailable,
            this, &MainWindow::onUpdateAvailable);
    connect(&UpdateManager::instance(), &UpdateManager::readyToInstall,
            this, &MainWindow::onReadyToInstall);
}

void MainWindow::onButtonClicked()
{
    ++m_clickCount;
    m_label->setText(QString("Clicked %1 time%2!")
                         .arg(m_clickCount)
                         .arg(m_clickCount == 1 ? "" : "s"));
}

void MainWindow::onUpdateAvailable(const QString &version,
                                   const QString &msiUrl,
                                   const QString &releaseNotes)
{
    // Show a non-intrusive status-bar button; user decides when to act.
    auto *btn = new QPushButton(
        QString("Update available: v%1 — click to install").arg(version),
        statusBar());
    btn->setFlat(true);
    statusBar()->addPermanentWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this, version, msiUrl, releaseNotes, btn] {
        const QString msg = releaseNotes.isEmpty()
            ? QString("Version %1 is available.\n\nDownload and install now?\n"
                      "The app will close to complete the update.").arg(version)
            : QString("Version %1 is available:\n\n%2\n\n"
                      "Download and install now? The app will close.").arg(version, releaseNotes);

        if (QMessageBox::question(this, "Update Available", msg) == QMessageBox::Yes) {
            btn->setEnabled(false);
            btn->setText("Downloading…");
            UpdateManager::instance().downloadUpdate(msiUrl);
        }
    });
}

void MainWindow::onReadyToInstall(const QString &installerPath)
{
#ifdef Q_OS_WIN
    // Detach msiexec so it keeps running after this process exits.
    QProcess::startDetached("msiexec", {"/i", installerPath, "/quiet", "/norestart"});
    QApplication::quit();
#else
    QMessageBox::information(this, "Update Ready",
        "Update downloaded to:\n" + installerPath +
        "\n\nPlease install it manually.");
#endif
}
