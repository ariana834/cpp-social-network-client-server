#include "ListFriendsCommand.h"
#include "Database.h"
#include "SessionManager.h"
#include <sstream>

std::string ListFriendsCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) return "ERROR Not logged in\n";

    std::string user = session.getUsername(client_sd);
    auto& db = Database::getInstance();

    auto friends = db.getFriends(user);
    if (friends.empty()) return "OK No friends\n";

    std::stringstream out;
    out << "OK Friends list:\n";

    for (auto& f : friends) {
        out << f.first << " " << f.second << "\n";
    }

    return out.str();
}
