#include "ShowCommand.h"
#include "SessionManager.h"
#include "Database.h"
#include <sstream>

std::string ShowCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();
    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";
    int conversation_id = sessions.getCurrentConversation(client_sd);
    if (conversation_id == -1)
        return "ERROR No open conversation";
    auto messages = db.getMessages(conversation_id);
    if (messages.empty())
        return "No messages";
    std::ostringstream out;
    for (const auto& m : messages)
        out << "[" << m.sender << "] " << m.content << "\n";
    return out.str();
}
