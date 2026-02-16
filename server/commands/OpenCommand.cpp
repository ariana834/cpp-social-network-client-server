#include "OpenCommand.h"
#include "SessionManager.h"
#include "Database.h"

OpenCommand::OpenCommand(int conversation_id)
    : conversation_id(conversation_id) {}

std::string OpenCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();

    // 1. autentificare
    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";

    std::string username = sessions.getUsername(client_sd);

    // 2. verificăm accesul la conversație
    if (!db.isParticipant(conversation_id, username))
        return "ERROR You are not a participant in this conversation";

    // 3. setăm conversația curentă (DOAR în sesiune)
    sessions.openConversation(client_sd, conversation_id);

    return "OK Opened conversation " + std::to_string(conversation_id);
}
