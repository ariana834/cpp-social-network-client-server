#pragma once

#include <QWidget>
#include <QString>

class QLineEdit;
class QPushButton;
class QLabel;

class LoginWindow : public QWidget {
    Q_OBJECT

signals:
    void loginSuccess(const QString& username);

public:
    explicit LoginWindow(QWidget *parent = nullptr);

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QPushButton *loginButton;
    QLabel *statusLabel;
    QPushButton* registerLink;
};
