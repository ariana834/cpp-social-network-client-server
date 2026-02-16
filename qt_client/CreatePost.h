#pragma once
#include <QDialog>

class QTextEdit;
class QLabel;
class QComboBox;

class CreatePost : public QDialog {

public:
    explicit CreatePost(QWidget* parent = nullptr);

private:
    QTextEdit* textEdit;
    QLabel* statusLabel;
    QComboBox* visibilityBox;

    void sendPost();
};
