#include "RegisterWindow.h"
#include "NetworkClient.h"
#include "LoginWindow.h"


#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Register");
    setFixedSize(800, 700);

    setStyleSheet("background-color: #fff0f6;");

    QLabel* title = new QLabel("Create account");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #ff4d8d;"
    );

    userEdit = new QLineEdit();
    userEdit->setPlaceholderText("Username");

    passEdit = new QLineEdit();
    passEdit->setPlaceholderText("Password");
    passEdit->setEchoMode(QLineEdit::Password);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Email");

    roleBox = new QComboBox();
    roleBox->addItem("normal");
    roleBox->addItem("admin");

    visibilityBox = new QComboBox();
    visibilityBox->addItem("public");
    visibilityBox->addItem("private");

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "  padding: 14px;"
        "  border-radius: 10px;"
        "  border: 1px solid #ffb3d9;"
        "  font-size: 14px;"
        "}";

    userEdit->setStyleSheet(inputStyle);
    passEdit->setStyleSheet(inputStyle);
    emailEdit->setStyleSheet(inputStyle);
    roleBox->setStyleSheet(inputStyle);
    visibilityBox->setStyleSheet(inputStyle);

    registerButton = new QPushButton("Register");
    registerButton->setFixedHeight(50);
    registerButton->setCursor(Qt::PointingHandCursor);
    registerButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #ff6fae;"
        "  color: white;"
        "  border-radius: 25px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ff4d8d;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #e60073;"
        "}"
    );

    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #cc0066;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(200, 0, 200, 0);

    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(emailEdit);
    layout->addWidget(roleBox);
    layout->addWidget(visibilityBox);
    layout->addWidget(registerButton);
    layout->addWidget(statusLabel);
    layout->addStretch();

    // ===== REGISTER LOGIC =====
    connect(registerButton, &QPushButton::clicked, this, [this]() {

        QString user = userEdit->text();
        QString pass = passEdit->text();
        QString email = emailEdit->text();
        QString role = roleBox->currentText();
        QString visibility = visibilityBox->currentText();

        if (user.isEmpty() || pass.isEmpty() || email.isEmpty()) {
            statusLabel->setText("Completează toate câmpurile");
            return;
        }

        auto& net = NetworkClient::instance();

        if (!net.connectToServer()) {
            statusLabel->setText("Server offline");
            return;
        }

        std::string cmd =
            "register " +
            user.toStdString() + " " +
            pass.toStdString() + " " +
            email.toStdString() + " " +
            role.toStdString() + " " +
            visibility.toStdString();

        std::string response = net.sendCommand(cmd);

        statusLabel->setText(QString::fromStdString(response));
        if (response.find("OK") != std::string::npos) {
    LoginWindow* login = new LoginWindow();
    login->show();
    this->close();
}

    });
}
