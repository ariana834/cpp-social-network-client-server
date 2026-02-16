#pragma once
#include "Command.h"

class ShowCommand : public Command {
public:
    ShowCommand() = default;
    std::string execute(int client_sd) override;
};
