#pragma once
#include "Command.h"

class ListFriendsCommand : public Command {
public:
    ListFriendsCommand() = default;
    std::string execute(int client_sd) override;
};
