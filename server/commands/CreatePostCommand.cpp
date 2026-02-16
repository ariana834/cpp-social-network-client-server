#include "CreatePostCommand.h"
#include "SessionManager.h"
#include "Database.h"

CreatePostCommand::CreatePostCommand(int vis,const std::string& cont)
    : visibility(vis), content(cont) {}

std::string CreatePostCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) return "ERROR Not logged in\n";
    if (content.empty()) return "ERROR Empty post\n";

    std::string user = session.getUsername(client_sd);
    if (!Database::getInstance().createPost(user, content, visibility))
        return "ERROR Cannot create post\n";

    return "OK Post created\n";
}
