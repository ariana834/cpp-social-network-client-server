#pragma once
#include "Command.h"
#include <string>

class AddCommand : public Command {
private:
    std::string username;

public:
    AddCommand(const std::string& username);
    std::string execute(int client_sd) override;
};
