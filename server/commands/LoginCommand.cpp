#include "LoginCommand.h"
#include "Database.h"
#include "SessionManager.h"
#include "Logger.h"

LoginCommand::LoginCommand(const std::string& user,
                           const std::string& pass)
    : username(user), password(pass) {}

std::string LoginCommand::execute(int client_sd) {
    Logger::getInstance().log(
        LogLevel::INFO,
        "LOGIN command received for user: " + username
    );

    if (username.empty() || password.empty()) {
        Logger::getInstance().log(LogLevel::WARNING,"LOGIN failed: missing credentials");
        return "ERROR Invalid arguments\n";
    }

    if (SessionManager::getInstance().isAuthenticated(client_sd)) {
        Logger::getInstance().log(LogLevel::WARNING,"LOGIN failed: client already authenticated (SD=" +std::to_string(client_sd) + ")");
        return "ERROR Already logged in\n";
    }
    auto userOpt = Database::getInstance().authenticateUser(username, password);
    if (!userOpt.has_value()) {
        Logger::getInstance().log(LogLevel::WARNING,"LOGIN failed: invalid credentials for user " + username);
        return "ERROR Invalid credentials\n";
    }

    SessionManager::getInstance().login(client_sd, username);
    Logger::getInstance().log(LogLevel::INFO,"User logged in successfully: " + username +" (type=" + userOpt->user_type + ")");
    return "OK Login successful\n";
}
