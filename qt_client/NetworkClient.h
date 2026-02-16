#pragma once
#include <string>

class NetworkClient {
public:
    static NetworkClient& instance();
    bool connectToServer();
    bool isConnected() const;
    std::string sendCommand(const std::string& cmd);
    void disconnect();
    void reconnect();
private:
    NetworkClient();
    int sock;
};
