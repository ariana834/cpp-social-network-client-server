#pragma once
#include "Command.h"

class LogoutCommand : public Command {
public:
    std::string execute(int client_sd) override;
};
