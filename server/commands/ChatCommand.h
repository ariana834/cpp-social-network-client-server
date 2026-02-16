#pragma once
#include "Command.h"
#include <string>

class ChatCommand : public Command {
private:
    std::string otherUser;

public:
    ChatCommand(const std::string& otherUser);
    std::string execute(int client_sd) override;
};
