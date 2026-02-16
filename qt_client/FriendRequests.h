#pragma once
#include <QDialog>

class QListWidget;
class QLabel;

class FriendRequests : public QDialog {
    Q_OBJECT

public:
    explicit FriendRequests(QWidget* parent = nullptr);

private:
    QListWidget* requestsList;
    QLabel* statusLabel;

    void loadRequests();
};
