#pragma once
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class GuestPostsWindow : public QWidget {
  Q_OBJECT
public:
  explicit GuestPostsWindow(QWidget *parent = nullptr);
private:
  QListWidget *postsList;
  QPushButton *backButton;
  void loadPublicPosts();
};
