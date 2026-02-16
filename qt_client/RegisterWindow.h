#pragma once
#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;
class QComboBox;

class RegisterWindow : public QWidget {
  Q_OBJECT

public:
  explicit RegisterWindow(QWidget *parent = nullptr);

private:
  QLineEdit *userEdit;
  QLineEdit *passEdit;
  QLineEdit *emailEdit;
  QComboBox *roleBox;
  QComboBox *visibilityBox;

  QPushButton *registerButton;
  QLabel *statusLabel;
};
