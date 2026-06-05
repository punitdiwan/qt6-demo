#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Called by SingleInstanceGuard when a second launch is attempted.
    void bringToFront();

private slots:
    void onButtonClicked();
    void onUpdateAvailable(const QString &version,
                           const QString &msiUrl,
                           const QString &releaseNotes);
    void onReadyToInstall(const QString &installerPath);

private:
    QLabel      *m_label;
    QPushButton *m_button;
    int          m_clickCount = 0;
};
