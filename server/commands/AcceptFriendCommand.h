#pragma once
#include "Command.h"
#include <string>

class AcceptFriendCommand : public Command {
private:
    std::string source;

public:
    explicit AcceptFriendCommand(const std::string& fromUser);
    std::string execute(int client_sd) override;
};
