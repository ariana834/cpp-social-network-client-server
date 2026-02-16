#include "ListFriendRequestsCommand.h"
#include "SessionManager.h"
#include "Database.h"
#include <sstream>

std::string ListFriendRequestsCommand::execute(int client_sd) {
    auto& session = SessionManager::getInstance();
    if (!session.isAuthenticated(client_sd)) return "ERROR Not logged in\n";

    std::string user = session.getUsername(client_sd);
    auto& db = Database::getInstance();

    auto requests = db.getFriendRequests(user);
    if (requests.empty()) return "OK No friend requests\n";

    std::stringstream out;
    out << "OK Friend requests:\n";
    for (auto& r : requests) out << r << "\n";

    return out.str();
}
