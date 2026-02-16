#include "ListPostsCommand.h"
#include "Database.h"
#include "SessionManager.h"
#include <sstream>

std::string ListPostsCommand::execute(int client_sd) {
  auto &session = SessionManager::getInstance();
  std::vector<Post> posts;
  if (session.isAuthenticated(client_sd)) {
    std::string user = session.getUsername(client_sd);
    posts = Database::getInstance().getVisiblePosts(user);
  } else {
    posts = Database::getInstance().getPublicPosts();
  }
  if (posts.empty())
    return "OK No posts\n";
  std::stringstream out;
  out << "OK Posts:\n";
  for (auto &p : posts) {
    out << "[" << p.created_at << "] ";
    out << p.author << " (" << p.visibility << "): ";
    out << p.content << "\n";
  }
  return out.str();
}
