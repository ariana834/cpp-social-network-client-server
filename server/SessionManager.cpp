#include "SessionManager.h"

SessionManager::SessionManager() {}

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

void SessionManager::login(int client_sd, const std::string& username) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    sessions[client_sd] = username;
}

void SessionManager::logout(int client_sd) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    sessions.erase(client_sd);
    currentConversation.erase(client_sd);
}

bool SessionManager::isAuthenticated(int client_sd) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    return sessions.find(client_sd) != sessions.end();
}

std::string SessionManager::getUsername(int client_sd) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    auto it = sessions.find(client_sd);
    if (it != sessions.end())
        return it->second;
    return "";
}
void SessionManager::openConversation(int client_sd,int conversation_id) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    currentConversation[client_sd] = conversation_id;
}

void SessionManager::closeConversation(int client_sd) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    currentConversation.erase(client_sd);
}

int SessionManager::getCurrentConversation(int client_sd) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (currentConversation.count(client_sd))
        return currentConversation[client_sd];
    return -1;
}
