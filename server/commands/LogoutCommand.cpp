#include "LogoutCommand.h"
#include "SessionManager.h"
#include "Logger.h"

std::string LogoutCommand::execute(int client_sd) {
    Logger::getInstance().log(
        LogLevel::INFO,
        "LOGOUT command received (SD=" + std::to_string(client_sd) + ")"
    );

    if (!SessionManager::getInstance().isAuthenticated(client_sd)) {
        Logger::getInstance().log(
            LogLevel::WARNING,
            "LOGOUT failed: client not authenticated (SD=" +
            std::to_string(client_sd) + ")"
        );
        return "ERROR Not logged in\n";
    }

    std::string username =
        SessionManager::getInstance().getUsername(client_sd);
    SessionManager::getInstance().logout(client_sd);

    Logger::getInstance().log(
        LogLevel::INFO,
        "User logged out: " + username +
        " (SD=" + std::to_string(client_sd) + ")"
    );
    return "OK Logout successful\n";
}
