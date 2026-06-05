#include "logindialog.h"
#include "logger.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

static constexpr const char *DEFAULT_EMAIL    = "admin@example.com";
static constexpr const char *DEFAULT_PASSWORD = "admin";

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Login — HelloQt6");
    setMinimumWidth(320);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // ── Form ────────────────────────────────────────────────────────────────
    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("Email address");
    m_emailEdit->setText(DEFAULT_EMAIL);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("Email:",    m_emailEdit);
    form->addRow("Password:", m_passwordEdit);

    // ── Error label (hidden until a failed attempt) ──────────────────────────
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();

    // ── Buttons ──────────────────────────────────────────────────────────────
    m_loginButton = new QPushButton("Log In", this);
    m_loginButton->setDefault(true);
    m_loginButton->setFixedWidth(100);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    btnRow->addWidget(m_loginButton);

    // ── Root layout ──────────────────────────────────────────────────────────
    auto *root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(24, 24, 24, 24);
    root->addLayout(form);
    root->addWidget(m_errorLabel);
    root->addLayout(btnRow);

    connect(m_loginButton, &QPushButton::clicked,
            this,          &LoginDialog::onLoginClicked);
    connect(m_emailEdit,   &QLineEdit::returnPressed,
            this,          &LoginDialog::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed,
            this,           &LoginDialog::onLoginClicked);
}

void LoginDialog::onLoginClicked()
{
    const QString email    = m_emailEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (email == DEFAULT_EMAIL && password == DEFAULT_PASSWORD) {
        Logger::write("Login", "Successful login: " + email);
        accept();
    } else {
        Logger::write("Login", "Failed login attempt for: " + email);
        m_errorLabel->setText("Invalid email or password.");
        m_errorLabel->show();
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}
