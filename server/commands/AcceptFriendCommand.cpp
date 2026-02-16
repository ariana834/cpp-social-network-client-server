#include "AcceptFriendCommand.h"
#include "Database.h"
#include "SessionManager.h"

AcceptFriendCommand::AcceptFriendCommand(const std::string& fromUser) : source(fromUser) {}

std::string AcceptFriendCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) return "ERROR Not logged in\n";

    std::string user = session.getUsername(client_sd);
    auto& db = Database::getInstance();

    if (!db.acceptFriendRequest(user, source)) return "ERROR Cannot accept request\n";
    return "OK Friend request accepted\n";
}
