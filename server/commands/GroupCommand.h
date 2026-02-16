#pragma once
#include "Command.h"
#include <vector>
#include <string>

class GroupCommand : public Command {
private:
    std::vector<std::string> users;

public:
    GroupCommand(const std::vector<std::string>& users);
    std::string execute(int client_sd) override;
};
