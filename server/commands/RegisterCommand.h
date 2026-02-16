#pragma once
#include "Command.h"
#include <string>

class RegisterCommand : public Command {
private:
  std::string username;
  std::string password;
  std::string email;
  std::string role;
  std::string visibility;

public:
  RegisterCommand(const std::string &u, const std::string &p,
                  const std::string &e, const std::string &r,
                  const std::string &v);

  std::string execute(int client_sd) override;
};
