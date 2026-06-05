#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    QLineEdit   *m_emailEdit;
    QLineEdit   *m_passwordEdit;
    QLabel      *m_errorLabel;
    QPushButton *m_loginButton;
};
