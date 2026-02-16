#pragma once
#include "Command.h"

class ListFriendRequestsCommand : public Command {
public:
    ListFriendRequestsCommand() = default;
    std::string execute(int client_sd) override;
};
