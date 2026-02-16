#include "SendCommand.h"
#include "Database.h"
#include "SessionManager.h"

SendCommand::SendCommand(const std::string &content) : content(content) {}

std::string SendCommand::execute(int client_sd) {
  auto &sessions = SessionManager::getInstance();
  auto &db = Database::getInstance();
  if (!sessions.isAuthenticated(client_sd))
    return "ERROR Not authenticated";
  int conversation_id = sessions.getCurrentConversation(client_sd);
  if (conversation_id == -1)
    return "ERROR No open conversation";
  std::string username = sessions.getUsername(client_sd);
  if (!db.sendMessage(conversation_id, username, content))
    return "ERROR Failed to send message";
  return "OK Message sent";
}
