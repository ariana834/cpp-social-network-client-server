#include "ChatsCommand.h"
#include "SessionManager.h"
#include "Database.h"
#include <sstream>

std::string ChatsCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();

    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";

    std::string username = sessions.getUsername(client_sd);

    auto conversations = db.getUserConversations(username);
    if (conversations.empty())
        return "No conversations";

    std::ostringstream out;

    for (int conv_id : conversations) {
        auto users = db.getConversationParticipants(conv_id);
        out << "[" << conv_id << "] ";
        bool first = true;
        for (const auto& u : users) {
            if (!first) out << ", ";
            out << u;
            first = false;
        }
        out << "\n";
    }

    return out.str();
}
