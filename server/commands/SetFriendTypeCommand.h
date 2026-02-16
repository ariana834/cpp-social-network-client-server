#pragma once
#include "Command.h"
#include <string>

class SetFriendTypeCommand : public Command {
private:
    std::string target;
    int type;

public:
    SetFriendTypeCommand(const std::string& user,int type);
    std::string execute(int client_sd) override;
};
