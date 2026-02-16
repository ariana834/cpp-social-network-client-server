#pragma once
#include <QTimer>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>

class MessagesWindow : public QWidget {
    Q_OBJECT

public:
    explicit MessagesWindow(QWidget* parent = nullptr);

private:
    QListWidget* conversationsList;
    QTextEdit* messagesView;
    QTextEdit* messageInput;
    QPushButton* sendButton;

    void loadMessages();
    void onConversationClicked(QListWidgetItem* item);
    void onSendClicked();
    void loadConversations();
    void onNewChatClicked();
    void onNewGroupClicked();


    QTimer* refreshTimer;
    int currentConversationId = -1;
};
