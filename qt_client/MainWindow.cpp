#include "MainWindow.h"
#include "NetworkClient.h"
#include "LoginWindow.h"
#include "AddFriend.h"
#include "FriendRequests.h"
#include "FriendType.h"
#include "CreatePost.h"

#include <QtWidgets>
#include <sstream>

#include "MessagesWindow.h"

void MainWindow::loadFriends() {
    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand("list_friends");
    friendsList->clear();
    if (response.find("No friends") != std::string::npos) {
        friendsList->addItem("(no friends yet)");
        return;
    }
    std::istringstream iss(response);
    std::string line;
    std::getline(iss, line);

    while (std::getline(iss, line)) {
        if (line.empty()) continue;

        std::istringstream ls(line);
        std::string username;
        int type;
        ls >> username >> type;

        QString label = (type == 1 ? "💖 " : type == 2 ? "👋 " : "🙂 ");
        label += QString::fromStdString(username);

        QListWidgetItem* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, QString::fromStdString(username));
        friendsList->addItem(item);
    }
}

void MainWindow::loadPosts(QVBoxLayout* layout) {
    auto& net = NetworkClient::instance();
    std::string response = net.sendCommand("list_posts");

    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (response.find("No posts") != std::string::npos) {
        QLabel* empty = new QLabel("No posts yet 💭");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("color:#cc0066; font-size:16px;");
        layout->addWidget(empty);
        layout->addStretch();
        return;
    }

    std::istringstream iss(response);
    std::string line;
    std::getline(iss, line);

    while (std::getline(iss, line)) {
        if (line.empty()) continue;

        QLabel* post = new QLabel(QString::fromStdString(line));
        post->setWordWrap(true);
        post->setStyleSheet(
            "background:white;"
            "border-radius:16px;"
            "padding:14px;"
            "font-size:14px;"
            "color:#cc0066;"
        );
        layout->addWidget(post);
    }
    layout->addStretch();
}

void MainWindow::refreshPosts() {
    loadPosts(postsLayout);
}

MainWindow::MainWindow(const QString& username, QWidget* parent)
    : QWidget(parent), currentUser(username)
{
    setWindowTitle("InstaLite 💕");
    setFixedSize(1000, 700);
    setStyleSheet("background-color:#fff0f6;");

    /* ===== TOP ===== */
    logoutButton = new QPushButton("Logout");
    logoutButton->setFixedSize(100, 40);
    logoutButton->setStyleSheet(
        "QPushButton{background:#ffb3d9;color:#cc0066;border-radius:20px;font-weight:bold;}"
        "QPushButton:hover{background:#ff99cc;}"
    );

    welcomeLabel = new QLabel("Welcome, " + currentUser + " 💖");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "font-size:40px;"
        "font-weight:900;"
        "color:#ff4d8d;"
    );

    /* ===== BUTTONS ===== */
    QPushButton* messagesButton = new QPushButton("💬");
    messagesButton->setFixedSize(48, 48);
    messagesButton->setCursor(Qt::PointingHandCursor);
    messagesButton->setStyleSheet(
        "QPushButton{"
        "background:#ff6fae;"
        "border-radius:24px;"
        "font-size:20px;"
        "color:white;"
        "}"
        "QPushButton:hover{background:#ff4d8d;}"
    );

    auto styleBtn = [](QPushButton* b){
        b->setFixedHeight(55);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton{background:#ffc1dc;color:#cc0066;border-radius:28px;font-size:16px;font-weight:bold;}"
            "QPushButton:hover{background:#ff99cc;}"
        );
    };

    QPushButton* addFriendButton = new QPushButton("➕ Add Friend");
    QPushButton* postButton = new QPushButton("✍️ Post");
    QPushButton* requestsButton = new QPushButton("Requests");
    QPushButton* reloadButton = new QPushButton("🔄");

    styleBtn(addFriendButton);
    styleBtn(postButton);
    styleBtn(requestsButton);

    reloadButton->setFixedSize(55,55);
    reloadButton->setStyleSheet(
        "QPushButton{background:#ff6fae;color:white;border-radius:27px;font-size:22px;}"
        "QPushButton:hover{background:#ff4d8d;}"
    );

    friendsList = new QListWidget();
    friendsList->setStyleSheet(
        "QListWidget{background:white;border-radius:16px;padding:6px;}"
        "QListWidget::item{padding:10px;border-radius:10px;}"
        "QListWidget::item:selected{background:#ffb3d9;color:white;}"
    );

    QVBoxLayout* friendsLayout = new QVBoxLayout();
    friendsLayout->addWidget(new QLabel("<b>Friends</b>"));
    friendsLayout->addWidget(requestsButton);
    friendsLayout->addWidget(friendsList);

    QWidget* friendsWidget = new QWidget();
    friendsWidget->setLayout(friendsLayout);
    friendsWidget->setFixedWidth(260);

    /* ===== POSTS ===== */
    QWidget* postsContent = new QWidget();
    postsLayout = new QVBoxLayout(postsContent);

    QScrollArea* postsScroll = new QScrollArea();
    postsScroll->setWidget(postsContent);
    postsScroll->setWidgetResizable(true);
    postsScroll->setStyleSheet("border:none;");

    QVBoxLayout* postsWrapper = new QVBoxLayout();
    postsWrapper->addWidget(new QLabel("<b>Posts</b>"));
    postsWrapper->addWidget(postsScroll);

    QWidget* postsWidget = new QWidget();
    postsWidget->setLayout(postsWrapper);


    //layout
    QHBoxLayout* actionsLayout = new QHBoxLayout();
    actionsLayout->addWidget(addFriendButton);
    actionsLayout->addWidget(postButton);
    actionsLayout->addWidget(reloadButton);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addWidget(friendsWidget);
    contentLayout->addWidget(postsWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->addWidget(logoutButton, 0, Qt::AlignLeft);
    topBar->addStretch();
    topBar->addWidget(welcomeLabel, 0, Qt::AlignCenter);
    topBar->addStretch();
    topBar->addWidget(messagesButton, 0, Qt::AlignRight);
    mainLayout->addLayout(topBar);
    mainLayout->addLayout(actionsLayout);
    mainLayout->addLayout(contentLayout);

    //logica
    connect(reloadButton, &QPushButton::clicked, this, &MainWindow::refreshPosts);

    connect(postButton, &QPushButton::clicked, this, [this]() {
        CreatePost dialog(this);
        if (dialog.exec() == QDialog::Accepted)
            refreshPosts();
    });

    connect(friendsList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        FriendType dialog(item->data(Qt::UserRole).toString(), this);
        if (dialog.exec() == QDialog::Accepted) {
            loadFriends();
            refreshPosts();
        }
    });

    connect(requestsButton, &QPushButton::clicked, this, [this]() {
        FriendRequests dialog(this);
        dialog.exec();
        loadFriends();
    });

    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        NetworkClient::instance().sendCommand("logout");
        LoginWindow* login = new LoginWindow();
        login->show();
        close();
    });
    connect(messagesButton, &QPushButton::clicked, this, [this]() {
       MessagesWindow* w = new MessagesWindow(this);
       w->show();
   });

    connect(addFriendButton, &QPushButton::clicked, this, [this]() {
        AddFriend dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            loadFriends();
            refreshPosts();
        }
    });
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, [this]() {
    loadFriends();
    refreshPosts();
});

    refreshTimer->start(5000);

    loadFriends();
    refreshPosts();
}

MainWindow::~MainWindow() {
    if (refreshTimer)
        refreshTimer->stop();
}
