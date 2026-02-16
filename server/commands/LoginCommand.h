#pragma once 
#include <string>
#include "Command.h"
using namespace std;

class LoginCommand : public Command {
    private: 
    string username;
    string password;
    public:
    LoginCommand(const string& user, const string& pass);
    string execute(int client_sd) override;
};

