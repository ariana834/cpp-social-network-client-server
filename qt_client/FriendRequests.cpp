#include "FriendRequests.h"
#include "NetworkClient.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <sstream>

FriendRequests::FriendRequests(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Requests");
    setFixedSize(380, 420);
    setStyleSheet("background-color: #fff0f6;");

    QLabel* title = new QLabel("Friend Requests");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #ff4d8d;"
    );

    requestsList = new QListWidget();
    requestsList->setStyleSheet(
        "QListWidget {"
        "  background: white;"
        "  border-radius: 12px;"
        "  padding: 6px;"
        "}"
        "QListWidget::item {"
        "  padding: 8px;"
        "}"
    );

    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "color: #cc0066;"
        "font-size: 12px;"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(20, 18, 20, 18);

    mainLayout->addWidget(title);
    mainLayout->addWidget(requestsList);
    mainLayout->addWidget(statusLabel);

    loadRequests();
}
void FriendRequests::loadRequests() {
    requestsList->clear();

    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand("list_friend_requests");

    if (response.find("OK") == std::string::npos) {
        statusLabel->setText("No requests");
        return;
    }

    std::stringstream ss(response);
    std::string line;

    std::getline(ss, line);

    while (std::getline(ss, line)) {
        if (line.empty()) continue;

        QListWidgetItem* item = new QListWidgetItem();
        QWidget* rowWidget = new QWidget();

        QLabel* nameLabel = new QLabel(QString::fromStdString(line));
        nameLabel->setStyleSheet(
            "font-weight: bold;"
            "color: #cc0066;"
        );

        QPushButton* acceptBtn = new QPushButton("✔");
        acceptBtn->setFixedSize(30, 30);
        acceptBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #ff6fae;"
            "  color: white;"
            "  border-radius: 15px;"
            "  font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #ff4d8d;"
            "}"
        );

        QPushButton* rejectBtn = new QPushButton("✖");
        rejectBtn->setFixedSize(30, 30);
        rejectBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #ffd6e8;"
            "  color: #cc0066;"
            "  border-radius: 15px;"
            "  font-size: 14px;"
            "}"
        );

        QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(8, 4, 8, 4);
        rowLayout->addWidget(nameLabel);
        rowLayout->addStretch();
        rowLayout->addWidget(acceptBtn);
        rowLayout->addWidget(rejectBtn);

        item->setSizeHint(QSize(0, 44));
        requestsList->addItem(item);
        requestsList->setItemWidget(item, rowWidget);

        //logica

        //butonul de accept
        connect(acceptBtn, &QPushButton::clicked, this, [this, line]() {
            auto& net = NetworkClient::instance();
            std::string resp =
                net.sendCommand("accept_friend " + line);

            statusLabel->setText(QString::fromStdString(resp));
            loadRequests();
        });

        //butonul de reject
        connect(rejectBtn, &QPushButton::clicked, this, [this]() {
            loadRequests();
        });
    }
}
