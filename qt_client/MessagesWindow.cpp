#include "MessagesWindow.h"
#include "NetworkClient.h"
#include <QHBoxLayout>
#include <sstream>
#include <QInputDialog>
#include <QLineEdit>

void MessagesWindow::onConversationClicked(QListWidgetItem* item) {
    int conversationId = item->data(Qt::UserRole).toInt();
    currentConversationId = conversationId;
    auto& net = NetworkClient::instance();
    net.sendCommand("open " + std::to_string(conversationId));
    loadMessages();
}

void MessagesWindow::onSendClicked() {
    auto& net = NetworkClient::instance();
    QString text = messageInput->toPlainText().trimmed();
    if (text.isEmpty())
        return;
    std::string response =
        net.sendCommand("send " + text.toStdString());
    if (response.find("OK") == std::string::npos) {
        messagesView->append("⚠️ Failed to send message");
        return;
    }
    messageInput->clear();
    std::string msgs = net.sendCommand("show");
    messagesView->clear();
    std::istringstream iss(msgs);
    std::string line;
    while (std::getline(iss, line))
        messagesView->append(QString::fromStdString(line));
}

void MessagesWindow::loadMessages() {
    if (currentConversationId == -1)
        return;
    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand("show");
    messagesView->clear();
    std::istringstream iss(response);
    std::string line;
    while (std::getline(iss, line)) {
        messagesView->append(QString::fromStdString(line));
    }
}

void MessagesWindow::loadConversations() {
    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand("chats");
    conversationsList->clear();
    std::istringstream iss(response);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty())
            continue;
        int l = line.find('[');
        int r = line.find(']');
        if (l == std::string::npos || r == std::string::npos)
            continue;

        int conversationId =
            std::stoi(line.substr(l + 1, r - l - 1));
        std::string users = line.substr(r + 2);
        int count = 1;
        for (char c : users)
            if (c == ',')
                count++;

        QString displayText;

        if (count == 2) {
            displayText = "💬 " + QString::fromStdString(users);
        } else {
            displayText =
                "👥 Group\n" +
                QString::fromStdString(users);
        }

        QListWidgetItem* item =
            new QListWidgetItem(displayText, conversationsList);
        item->setData(Qt::UserRole, conversationId);
    }
}

void MessagesWindow::onNewChatClicked() {
    bool ok;
    QString user =
        QInputDialog::getText(this,
                              "New Chat 💬",
                              "Username:",
                              QLineEdit::Normal,
                              "",
                              &ok);

    if (!ok || user.isEmpty())
        return;
    auto& net = NetworkClient::instance();
    net.sendCommand("chat " + user.toStdString());
    loadConversations();
}
void MessagesWindow::onNewGroupClicked() {
    bool ok;
    QString users =
        QInputDialog::getText(this,
                              "New Group 👥",
                              "Users (space separated):",
                              QLineEdit::Normal,
                              "",
                              &ok);
    if (!ok || users.isEmpty())
        return;
    auto& net = NetworkClient::instance();
    net.sendCommand("group " + users.toStdString());
    loadConversations();
}


MessagesWindow::MessagesWindow(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("Messages 💬");
    resize(700, 450);
    setWindowFlags(Qt::Window);
    conversationsList = new QListWidget(this);
    QPushButton* newChatButton = new QPushButton("➕ New Chat", this);
    QPushButton* newGroupButton = new QPushButton("👥 New Group", this);

    QString pinkButtonStyle = R"(
        QPushButton {
            background-color: #ff7eb9;
            color: white;
            border-radius: 12px;
            padding: 6px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #ff5fa2;
        }
        QPushButton:pressed {
            background-color: #e94c8f;
        }
    )";

    newChatButton->setStyleSheet(pinkButtonStyle);
    newGroupButton->setStyleSheet(pinkButtonStyle);

    //layout
    QHBoxLayout* topButtonsLayout = new QHBoxLayout;
    topButtonsLayout->addWidget(newChatButton);
    topButtonsLayout->addWidget(newGroupButton);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topButtonsLayout);
    leftLayout->addWidget(conversationsList);

    messagesView = new QTextEdit(this);
    messagesView->setReadOnly(true);

    messageInput = new QTextEdit(this);
    messageInput->setFixedHeight(60);

    sendButton = new QPushButton("Send 💌", this);
    sendButton->setStyleSheet(pinkButtonStyle);

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(messagesView);
    rightLayout->addWidget(messageInput);
    rightLayout->addWidget(sendButton);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 3);

    setLayout(mainLayout);

    //logica
    loadConversations();

    connect(conversationsList, &QListWidget::itemClicked,
            this, &MessagesWindow::onConversationClicked);

    connect(sendButton, &QPushButton::clicked,
            this, &MessagesWindow::onSendClicked);
    connect(newChatButton, &QPushButton::clicked,
        this, &MessagesWindow::onNewChatClicked);

    connect(newGroupButton, &QPushButton::clicked,
            this, &MessagesWindow::onNewGroupClicked);


    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout,
            this, &MessagesWindow::loadMessages);
    refreshTimer->start(2000);

}
