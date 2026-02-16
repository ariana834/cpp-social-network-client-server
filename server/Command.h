#pragma once 
#include <string>
using namespace std;

class Command {
    public:
    virtual string execute(int client_sd)=0;
    virtual ~Command() {}
};