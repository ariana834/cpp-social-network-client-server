#pragma once
#include "Command.h"

class OpenCommand : public Command {
private:
    int conversation_id;

public:
    OpenCommand(int conversation_id);
    std::string execute(int client_sd) override;
};
