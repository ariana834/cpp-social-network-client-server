#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "CommandFactory.h"
#include "Logger.h"
#include "SessionManager.h"
using namespace std;

#define PORT 8080
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

void error_exit(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == 0) {
    error_exit("socket failed");
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    error_exit("bind failed");
  }
  if (listen(server_socket, 3) < 0) {
    error_exit("listen");
  }

  Logger::getInstance().log(LogLevel::INFO,
                            "Server started. Listening on port " +
                                std::to_string(PORT));

  fd_set master_set, read_set, write_set;
  FD_ZERO(&master_set);
  FD_SET(server_socket, &master_set);

  int activity, new_socket, sd, max_sd, valread;
  int client_socket[MAX_CLIENTS] = {0};
  char buffer[BUFFER_SIZE];
  socklen_t addrlen;

  while (true) {
    read_set = master_set;
    write_set = master_set;

    max_sd = server_socket;
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_socket[i] > max_sd)
        max_sd = client_socket[i];
    }

    activity = select(max_sd + 1, &read_set, &write_set, NULL, NULL);
    if (activity < 0) {
      Logger::getInstance().log(LogLevel::ERROR, "Select error");
      continue;
    }

    if (FD_ISSET(server_socket, &read_set)) {
      addrlen = sizeof(address);
      new_socket = accept(server_socket, (struct sockaddr *)&address, &addrlen);
      if (new_socket < 0) {
        error_exit("accept");
      }

      Logger::getInstance().log(
          LogLevel::INFO,
          "New client connected. SD=" + to_string(new_socket) +
              ", IP=" + std::string(inet_ntoa(address.sin_addr)));

      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_socket[i] == 0) {
          client_socket[i] = new_socket;
          break;
        }
      }

      FD_SET(new_socket, &master_set);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
      sd = client_socket[i];

      if (sd > 0 && FD_ISSET(sd, &read_set)) {
        memset(buffer, 0, BUFFER_SIZE);
        valread = read(sd, buffer, BUFFER_SIZE - 1);

        if (valread == 0) {
          Logger::getInstance().log(LogLevel::INFO,
                                    "Client disconnected. SD=" + to_string(sd));
          SessionManager::getInstance().logout(sd);
          close(sd);
          FD_CLR(sd, &master_set);
          client_socket[i] = 0;
        } else {
          string input(buffer);
          input.erase(input.find_last_not_of("\n\r") + 1);
          // nu si alea care sunt polling
          if (input != "list_posts" && input != "list_friends" && input != "show") {
            Logger::getInstance().log(LogLevel::INFO, input);
          }
          Command *command = CommandFactory::create(input);
          string response;

          if (!command) {
            response = "ERROR Unknown command\n";
            Logger::getInstance().log(LogLevel::WARNING,
                                      "Unknown command: " + input);
          } else {
            response = command->execute(sd);
            delete command;
          }
          send(sd, response.c_str(), response.size(), 0);
        }
      }
    }
  }
}
