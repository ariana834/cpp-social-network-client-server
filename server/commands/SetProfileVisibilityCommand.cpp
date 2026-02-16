#include "SetProfileVisibilityCommand.h"
#include "Database.h"
#include "Logger.h"
#include "SessionManager.h"

SetProfileVisibilityCommand::SetProfileVisibilityCommand(std::string &v)
    : visibility(v) {}

std::string SetProfileVisibilityCommand::execute(int client_sd) {
  Logger::getInstance().log(
      LogLevel::INFO, "SET_PROFILE_VISIBILITY command received: " + visibility);
  if (!SessionManager::getInstance().isAuthenticated(client_sd)) {
    Logger::getInstance().log(
        LogLevel::WARNING,
        "SET_PROFILE_VISIBILITY failed: client not authenticated");
    return "ERROR Not logged in\n";
  }
  if (visibility != "public" && visibility != "private") {
    Logger::getInstance().log(LogLevel::WARNING,
                              "SET_PROFILE_VISIBILITY failed: invalid value (" +
                                  visibility + ")");
    return "ERROR Invalid visibility\n";
  }
  std::string username = SessionManager::getInstance().getUsername(client_sd);
  auto userOpt = Database::getInstance().getUserByUsername(username);
  if (!userOpt.has_value()) {
    Logger::getInstance().log(
        LogLevel::ERROR,
        "SET_PROFILE_VISIBILITY failed: user not found in DB (" + username +
            ")");
    return "ERROR Internal error\n";
  }
  Database::getInstance().updateProfileVisibility(userOpt->user_id, visibility);
  Logger::getInstance().log(LogLevel::INFO,
                            "Profile visibility updated for user " + username +
                                " to " + visibility);
  return "OK Profile visibility updated\n";
}
