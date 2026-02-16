#include "GroupCommand.h"
#include "SessionManager.h"
#include "Database.h"
#include <sstream>

GroupCommand::GroupCommand(const std::vector<std::string>& users)
    : users(users) {}

std::string GroupCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();
    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";
    std::string me = sessions.getUsername(client_sd);
    std::vector<std::string> allUsers = users;
    allUsers.push_back(me);
    for (const auto& u : allUsers) {
        if (!db.getUserByUsername(u).has_value())
            return "ERROR User does not exist: " + u;
    }
    int conv = db.createGroupConversation(allUsers);
    if (conv == -1)
        return "ERROR Failed to create group";
    sessions.openConversation(client_sd, conv);
    return "OK Group created. conversation_id = " + std::to_string(conv);
}
