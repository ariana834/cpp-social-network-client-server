#pragma once
#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class AddFriend : public QDialog {
    Q_OBJECT

public:
    explicit AddFriend(QWidget* parent = nullptr);

private:
    QLineEdit* userEdit;
    QLabel* statusLabel;
};
