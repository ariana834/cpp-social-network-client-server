#pragma once
#include "Command.h"

class ListPostsCommand : public Command {
public:
    std::string execute(int client_sd) override;
};
