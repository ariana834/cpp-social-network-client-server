#include "SetFriendTypeCommand.h"
#include "Database.h"
#include "SessionManager.h"

SetFriendTypeCommand::SetFriendTypeCommand(const std::string& user,int t) : target(user), type(t) {}

std::string SetFriendTypeCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) return "ERROR Not logged in\n";

    std::string user = session.getUsername(client_sd);
    auto& db = Database::getInstance();

    if (!db.setFriendType(user, target, type)) return "ERROR Cannot set friend type\n";
    return "OK Friend type updated\n";
}
