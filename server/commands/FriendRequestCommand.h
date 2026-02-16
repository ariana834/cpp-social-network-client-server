#pragma once
#include "Command.h"
#include <string>

class FriendRequestCommand : public Command {
private:
    std::string target;

public:
    explicit FriendRequestCommand(const std::string& targetUser);
    std::string execute(int client_sd) override;
};
