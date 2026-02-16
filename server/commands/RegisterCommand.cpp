#include "RegisterCommand.h"
#include "Database.h"
#include "Logger.h"

RegisterCommand::RegisterCommand(const std::string& u,
                                 const std::string& p,
                                 const std::string& e,
                                 const std::string& r,
                                 const std::string& v)
    : username(u), password(p), email(e), role(r), visibility(v) {}

std::string RegisterCommand::execute(int) {
    Logger::getInstance().log(
        LogLevel::INFO,
        "REGISTER attempt: " + username
    );

    if (username.empty() || password.empty() || email.empty()
        || role.empty() || visibility.empty()) {
        Logger::getInstance().log(
            LogLevel::WARNING,
            "REGISTER failed: missing arguments"
        );
        return "ERROR Invalid arguments\n";
    }

    if (role != "normal" && role != "admin") {
        Logger::getInstance().log(
            LogLevel::WARNING,
            "REGISTER failed: invalid role (" + role + ")"
        );
        return "ERROR Invalid role\n";
    }

    if (visibility != "public" && visibility != "private") {
        Logger::getInstance().log(
            LogLevel::WARNING,
            "REGISTER failed: invalid visibility (" + visibility + ")"
        );
        return "ERROR Invalid visibility\n";
    }

    bool ok = Database::getInstance().registerUser(
        username, password, email, role
    );

    if (!ok) {
        Logger::getInstance().log(
            LogLevel::WARNING,
            "REGISTER failed: user already exists (" + username + ")"
        );
        return "ERROR User already exists\n";
    }

    // setezi vizibilitatea explicit (nu default)
    auto userOpt = Database::getInstance().authenticateUser(username, password);
    if (userOpt.has_value()) {
        Database::getInstance().updateProfileVisibility(
            userOpt->user_id, visibility
        );
    }

    Logger::getInstance().log(
        LogLevel::INFO,
        "User registered successfully: " + username +
        " role=" + role +
        " visibility=" + visibility
    );

    return "OK User registered\n";
}
