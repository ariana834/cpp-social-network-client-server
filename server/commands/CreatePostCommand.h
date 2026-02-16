#pragma once
#include "Command.h"
#include <string>

class CreatePostCommand : public Command {
private:
    int visibility;
    std::string content;

public:
    CreatePostCommand(int vis,const std::string& cont);
    std::string execute(int client_sd) override;
};
