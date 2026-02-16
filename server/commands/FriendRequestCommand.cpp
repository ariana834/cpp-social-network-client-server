#include "FriendRequestCommand.h"
#include "Database.h"
#include "SessionManager.h"

FriendRequestCommand::FriendRequestCommand(const std::string& t) : target(t) {}

std::string FriendRequestCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) {
        return "ERROR Not logged in\n";
    }
    std::string user = session.getUsername(client_sd);
    if (user == target) {
        return "ERROR Cannot add yourself\n";
    }
    auto& db = Database::getInstance();
    if (!db.getUserByUsername(target).has_value()) {
        return "ERROR User does not exist\n";
    }
    if (db.areFriends(user, target)) {
        return "ERROR Already friends\n";
    }
    if (!db.sendFriendRequest(user, target)) {
        return "ERROR Cannot send request\n";
    }
    return "OK Friend request sent\n";
}
