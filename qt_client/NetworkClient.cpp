#include "NetworkClient.h"

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_HOST "localhost"
#define SERVER_PORT "8080"

NetworkClient::NetworkClient() : sock(-1) {}

NetworkClient& NetworkClient::instance() {
    static NetworkClient instance;
    return instance;
}

bool NetworkClient::connectToServer() {
    if (sock != -1)
        return true;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return false;
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr) <= 0) {
        close(sock);
        sock = -1;
        return false;
    }
    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        close(sock);
        sock = -1;
        return false;
    }
    return true;
}


bool NetworkClient::isConnected() const {
    return sock != -1;
}

std::string NetworkClient::sendCommand(const std::string& cmd) {
    if (sock == -1)
        return "ERROR Not connected\n";
    std::string fullCmd = cmd;
    if (fullCmd.back() != '\n')
        fullCmd += '\n';
    ssize_t sent = send(sock, fullCmd.c_str(), fullCmd.size(), 0);
    if (sent <= 0) {
        disconnect();
        return "ERROR Send failed\n";
    }
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        return "ERROR Server did not respond\n";
    }
    buffer[bytes] = '\0';
    return std::string(buffer);
}


void NetworkClient::disconnect() {
    if (sock != -1) {
        ::close(sock);
        sock = -1;
    }
}
void NetworkClient::reconnect() {
    disconnect();
    connectToServer();
}
