#include "GuestPostsWindow.h"
#include "NetworkClient.h"
#include "LoginWindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>

GuestPostsWindow::GuestPostsWindow(QWidget* parent)
    : QWidget(parent)
{
  setWindowTitle("Public Posts");
  setFixedSize(800, 700);
  setStyleSheet("background-color: #fff0f6;");

  QLabel* title = new QLabel("🌸 Public posts");
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet(
      "font-size: 26px;"
      "font-weight: bold;"
      "color: #ff4d8d;"
  );
  postsList = new QListWidget();
  postsList->setStyleSheet(
      "QListWidget {"
      "  background: white;"
      "  border-radius: 12px;"
      "  padding: 10px;"
      "}"
      "QListWidget::item {"
      "  padding: 10px;"
      "  border-bottom: 1px solid #ffe0ef;"
      "}"
  );
  backButton = new QPushButton("← Back to login");
  backButton->setCursor(Qt::PointingHandCursor);
  backButton->setFixedWidth(180);
  backButton->setStyleSheet(
      "QPushButton {"
      "  background-color: #ffd6e8;"
      "  color: #cc0066;"
      "  border-radius: 18px;"
      "  padding: 8px 16px;"
      "  font-weight: bold;"
      "}"
      "QPushButton:hover {"
      "  background-color: #ffb3d9;"
      "}"
  );

  //layout
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setSpacing(15);
  layout->setContentsMargins(40, 30, 40, 30);
  layout->addWidget(title);
  layout->addWidget(postsList);
  layout->addWidget(backButton, 0, Qt::AlignCenter);


  //logica
  loadPublicPosts();
  connect(backButton, &QPushButton::clicked, this, [this]() {
      LoginWindow* login = new LoginWindow();
      login->show();
      this->close();
  });
}
void GuestPostsWindow::loadPublicPosts() {
    auto& net = NetworkClient::instance();

    if (!net.connectToServer()) {
        postsList->addItem("❌ Server offline");
        return;
    }
    std::string response = net.sendCommand("list_posts");
    postsList->clear();
    QString resp = QString::fromStdString(response);
    if (!resp.startsWith("OK")) {
        postsList->addItem("❌ Failed to load posts");
        return;
    }
    QStringList lines = resp.split('\n');
    for (int i = 1; i < lines.size(); ++i) {
        if (!lines[i].trimmed().isEmpty())
            postsList->addItem(lines[i]);
    }
    if (postsList->count() == 0) {
        postsList->addItem("No public posts yet 🌸");
    }
}
