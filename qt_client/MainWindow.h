#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QTimer>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(const QString& username, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void refreshPosts();

private:
    void loadFriends();
    void loadPosts(QVBoxLayout* layout);

    QString currentUser;
    QPushButton* logoutButton;
    QLabel* welcomeLabel;
    QListWidget* friendsList;
    QVBoxLayout* postsLayout;
    QTimer* refreshTimer;  // Timer pentru auto-refresh
};

#endif // MAINWINDOW_H