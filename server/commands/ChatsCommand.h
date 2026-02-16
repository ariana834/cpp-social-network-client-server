#pragma once
#include "Command.h"

class ChatsCommand : public Command {
public:
    ChatsCommand() = default;
    std::string execute(int client_sd) override;
};
