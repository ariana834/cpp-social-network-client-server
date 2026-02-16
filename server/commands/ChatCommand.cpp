#include "ChatCommand.h"
#include "Database.h"
#include "SessionManager.h"

ChatCommand::ChatCommand(const std::string& otherUser)
    : otherUser(otherUser) {}
std::string ChatCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();
    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";
    std::string username = sessions.getUsername(client_sd);
    if (username == otherUser)
        return "ERROR Cannot chat with yourself";
    if (!db.getUserByUsername(otherUser).has_value())
        return "ERROR User does not exist";

    auto existing = db.getPrivateConversation(username, otherUser);
    if (existing.has_value()) {
        return "OK Chat already exists. conversation_id = " +
               std::to_string(existing.value());
    }
    int conversation_id = db.createConversation(username, otherUser);
    if (conversation_id < 0)
        return "ERROR Failed to create chat";

    return "OK Private chat created. conversation_id = " +
           std::to_string(conversation_id);
}
