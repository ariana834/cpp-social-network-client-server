#pragma once
#include <unordered_map>
#include <string>
#include <mutex>

class SessionManager {
private:
    std::unordered_map<int, std::string> sessions;
    std::unordered_map<int, int> currentConversation;
    std::mutex sessionMutex;

    SessionManager();
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

public:
    static SessionManager& getInstance();

    void login(int client_sd,const std::string& username);
    void logout(int client_sd);

    bool isAuthenticated(int client_sd);
    std::string getUsername(int client_sd);

    void openConversation(int client_sd,int conversation_id);
    void closeConversation(int client_sd);
    int getCurrentConversation(int client_sd);
};
