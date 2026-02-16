#include "CreatePost.h"
#include "NetworkClient.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

CreatePost::CreatePost(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Create Post");
    setFixedSize(400, 300);
    setStyleSheet("background-color: #fff0f6;");

    QLabel* title = new QLabel("Post your thoughts ✍️");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-weight: bold; color: #ff4d8d;");

    textEdit = new QTextEdit();
    textEdit->setPlaceholderText("What's on your mind? 💭");

    visibilityBox = new QComboBox();
    visibilityBox->addItem("🌍 Public", 0);
    visibilityBox->addItem("👥 Friends", 1);
    visibilityBox->addItem("🔒 Private", 2);

    QPushButton* postBtn = new QPushButton("Post");
    postBtn->setStyleSheet(
        "QPushButton { background:#ff6fae; color:white; border-radius:16px; padding:8px; }"
        "QPushButton:hover { background:#ff4d8d; }"
    );

    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size:11px; color:#cc0066;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->addWidget(title);
    layout->addWidget(textEdit);
    layout->addWidget(visibilityBox);
    layout->addWidget(postBtn);
    layout->addWidget(statusLabel);

    connect(postBtn, &QPushButton::clicked, this, [this]() {
        sendPost();
    });
}

void CreatePost::sendPost() {
    QString content = textEdit->toPlainText().trimmed();
    if (content.isEmpty()) {
        statusLabel->setText("Post cannot be empty 😅");
        return;
    }

    int vis = visibilityBox->currentData().toInt();

    std::string cmd =
        "create_post " +
        std::to_string(vis) + " " +
        content.toStdString();

    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand(cmd);

    statusLabel->setText(QString::fromStdString(response));

    if (response.find("OK") != std::string::npos) {
        accept();
    }
}
