#include "AddFriend.h"
#include "NetworkClient.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QTimer>

AddFriend::AddFriend(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("AddFriend");
    setFixedSize(400, 300);
    setStyleSheet("background-color: #fff0f6;");

    QLabel* title = new QLabel("AddFriend");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 25px;"
        "font-weight: bold;"
        "color: #ff4d8d;"
    );

    userEdit = new QLineEdit();
    userEdit->setPlaceholderText("Username");
    userEdit->setFixedHeight(32);
    userEdit->setStyleSheet(
        "QLineEdit {"
        "  padding: 7px;"
        "  border-radius: 9px;"
        "  border: 1px solid #ffb3d9;"
        "  font-size: 15px;"
        "}"
    );

    QPushButton* addButton = new QPushButton("Add");
    addButton->setFixedHeight(34);
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #ff6fae;"
        "  color: white;"
        "  border-radius: 17px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ff4d8d;"
        "}"
    );

    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "color: #cc0066;"
        "font-size: 11px;"
    );

    //layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(15, 12, 15, 12);

    layout->addWidget(title);
    layout->addWidget(userEdit);
    layout->addWidget(addButton);
    layout->addWidget(statusLabel);

   //logic
    connect(addButton, &QPushButton::clicked, this, [this]() {
        QString username = userEdit->text();

        if (username.isEmpty()) {
            statusLabel->setText("Enter username");
            return;
        }

        auto& net = NetworkClient::instance();
        std::string response =
            net.sendCommand("add_friend " + username.toStdString());

        statusLabel->setText(QString::fromStdString(response));

        if (response.find("OK") != std::string::npos) {
            QTimer::singleShot(800, this, &QDialog::accept);
        }
    });
}
