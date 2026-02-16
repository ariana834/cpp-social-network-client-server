#pragma once
#include "Command.h"
#include <string>

class SetProfileVisibilityCommand : public Command {
private:
    std::string visibility;

public:
    SetProfileVisibilityCommand(std::string& v);
    std::string execute(int client_sd) override;
};
