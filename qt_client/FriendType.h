#pragma once
#include <QDialog>

class QLabel;
class QPushButton;

class FriendType : public QDialog {
    Q_OBJECT

public:
    explicit FriendType(const QString& friendName, QWidget* parent = nullptr);

private:
    QString friendUser;
    QLabel* statusLabel;

    void sendType(int type);
};
