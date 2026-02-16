#pragma once
#include "Command.h"
#include <string>

class SendCommand : public Command {
private:
  std::string content;

public:
  SendCommand(const std::string &content);
  std::string execute(int client_sd) override;
};
