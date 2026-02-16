#include "AddCommand.h"
#include "SessionManager.h"
#include "Database.h"

AddCommand::AddCommand(const std::string& username)
    : username(username) {}

std::string AddCommand::execute(int client_sd) {
    auto& sessions = SessionManager::getInstance();
    auto& db = Database::getInstance();
    if (!sessions.isAuthenticated(client_sd))
        return "ERROR Not authenticated";
    int conv = sessions.getCurrentConversation(client_sd);
    if (conv == -1)
        return "ERROR No open conversation";
    if (!db.getUserByUsername(username).has_value())
        return "ERROR User does not exist";
    if (!db.addParticipant(conv, username))
        return "ERROR Failed to add user";
    return "OK User added to group";
}
