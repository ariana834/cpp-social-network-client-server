#pragma once
#include <string>
#include "Command.h"
using namespace std;

class CommandFactory {
public:
    static Command* create(const string& input);
};
