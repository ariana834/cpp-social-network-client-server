#include "FriendType.h"
#include "NetworkClient.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QTimer>

FriendType::FriendType(const QString& friendName, QWidget* parent)
    : QDialog(parent), friendUser(friendName)
{
    setWindowTitle("Friend Type");
    setFixedSize(260, 220);
    setStyleSheet("background-color: #fff0f6;");

    QLabel* title = new QLabel("Set friend type");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 15px;"
        "font-weight: bold;"
        "color: #ff4d8d;"
    );

    QLabel* nameLabel = new QLabel(friendUser);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "font-weight: bold;"
        "color: #cc0066;"
    );

    QPushButton* normalBtn = new QPushButton("🙂 Normal");
    QPushButton* closeBtn  = new QPushButton("💖 Close friend");
    QPushButton* knownBtn  = new QPushButton("👋 Cunoștință");

    QString btnStyle =
        "QPushButton {"
        "  background-color: #ffb3d9;"
        "  border-radius: 18px;"
        "  padding: 8px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ff99cc;"
        "}";

    normalBtn->setStyleSheet(btnStyle);
    closeBtn->setStyleSheet(btnStyle);
    knownBtn->setStyleSheet(btnStyle);

    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 11px; color: #cc0066;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(18, 15, 18, 15);

    layout->addWidget(title);
    layout->addWidget(nameLabel);
    layout->addWidget(normalBtn);
    layout->addWidget(closeBtn);
    layout->addWidget(knownBtn);
    layout->addWidget(statusLabel);

    connect(normalBtn, &QPushButton::clicked, this, [this]() { sendType(0); });
    connect(closeBtn,  &QPushButton::clicked, this, [this]() { sendType(1); });
    connect(knownBtn,  &QPushButton::clicked, this, [this]() { sendType(2); });
}

void FriendType::sendType(int type) {
    auto& net = NetworkClient::instance();

    std::string cmd =
        "set_friend_type " +
        friendUser.toStdString() + " " +
        std::to_string(type);

    std::string response = net.sendCommand(cmd);

    statusLabel->setText(QString::fromStdString(response));

    if (response.find("OK") != std::string::npos) {
        accept();
    }
}

