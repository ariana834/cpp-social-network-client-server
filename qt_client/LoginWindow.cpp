#include "LoginWindow.h"

#include "GuestPostsWindow.h"
#include "MainWindow.h"
#include "NetworkClient.h"
#include "RegisterWindow.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
  setWindowTitle("InstaLite");
  setFixedSize(800, 700);

  setStyleSheet("background-color: #fff0f6;");

  QLabel *title = new QLabel("InstaLite");
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("font-size: 30px;"
                       "font-weight: bold;"
                       "color: #ff4d8d;");

  userEdit = new QLineEdit();
  userEdit->setPlaceholderText("Username");

  passEdit = new QLineEdit();
  passEdit->setPlaceholderText("Password");
  passEdit->setEchoMode(QLineEdit::Password);

  QString inputStyle = "QLineEdit {"
                       "  padding: 14px;"
                       "  border-radius: 10px;"
                       "  border: 1px solid #ffb3d9;"
                       "  font-size: 14px;"
                       "}";

  userEdit->setStyleSheet(inputStyle);
  passEdit->setStyleSheet(inputStyle);

  loginButton = new QPushButton("Login");
  loginButton->setFixedHeight(50);
  loginButton->setCursor(Qt::PointingHandCursor);
  loginButton->setStyleSheet("QPushButton {"
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
                             "}");

  statusLabel = new QLabel("");
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setStyleSheet("color: #cc0066;");

  //aici e si butonul de register
  QLabel *registerText = new QLabel("You don’t have an account?");
  registerText->setAlignment(Qt::AlignCenter);
  registerText->setStyleSheet("color: #cc0066;");

  registerLink = new QPushButton("Create one");
  registerLink->setCursor(Qt::PointingHandCursor);
  registerLink->setFlat(true);
  registerLink->setStyleSheet("QPushButton {"
                              "  color: #ff4d8d;"
                              "  font-weight: bold;"
                              "  background: transparent;"
                              "  text-decoration: underline;"
                              "}"
                              "QPushButton:hover {"
                              "  color: #e60073;"
                              "}");
  QPushButton* viewPostsBtn = new QPushButton("💗 View posts without login");
  viewPostsBtn->setCursor(Qt::PointingHandCursor);
  viewPostsBtn->setFixedHeight(48);
  viewPostsBtn->setMinimumWidth(260);

  viewPostsBtn->setStyleSheet(
     "QPushButton {"
     "  background-color: #ffb3d9;"
     "  color: white;"
     "  border-radius: 24px;"
     "  padding: 10px 24px;"
     "  font-size: 15px;"
     "  font-weight: bold;"
     "  border: none;"
     "}"
     "QPushButton:hover {"
     "  background-color: #ff8fc4;"
     "}"
     "QPushButton:pressed {"
     "  background-color: #ff6fae;"
     "}"
 );

  //layout
  QHBoxLayout* topLayout = new QHBoxLayout();
  topLayout->addWidget(viewPostsBtn);
  topLayout->addStretch();

  QVBoxLayout* centerLayout = new QVBoxLayout();
  centerLayout->setSpacing(20);
  centerLayout->setContentsMargins(200, 0, 200, 0);

  centerLayout->addStretch();
  centerLayout->addWidget(title);
  centerLayout->addWidget(userEdit);
  centerLayout->addWidget(passEdit);
  centerLayout->addWidget(loginButton);
  centerLayout->addWidget(statusLabel);
  centerLayout->addWidget(registerText);
  centerLayout->addWidget(registerLink);
  centerLayout->addStretch();
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins(20, 20, 20, 20);

  layout->addLayout(topLayout);
  layout->addLayout(centerLayout);

  //connect(obiect, semnal, cine, ce functie);
  connect(loginButton, &QPushButton::clicked, this, [this]() {
    QString username = userEdit->text();
    QString password = passEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
      statusLabel->setText("Completează toate câmpurile");
      return;
    }

    auto &net = NetworkClient::instance();

    if (!net.connectToServer()) {
      statusLabel->setText("Server offline");
      return;
    }

    std::string response = net.sendCommand("login " + username.toStdString() +
                                           " " + password.toStdString());

    statusLabel->setText(QString::fromStdString(response));
    if (response.find("OK") != std::string::npos) {
      MainWindow *main = new MainWindow(username);
      main->show();
      this->close();
    }
  });

  connect(registerLink, &QPushButton::clicked, this, [this]() {
    RegisterWindow *reg = new RegisterWindow();
    reg->show();
    this->close();
  });

  connect(viewPostsBtn, &QPushButton::clicked, this, [this]() {
    GuestPostsWindow* guest = new GuestPostsWindow();
    guest->show();
    this->close();
});

}
