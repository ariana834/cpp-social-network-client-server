#include "Logger.h"
#include <iostream>
#include <ctime>
using namespace std;
Logger::Logger() {
    logFile.open("server.log", std::ios::app);
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

string Logger::currentTimestamp() {
    time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer),"%Y-%m-%d %H:%M:%S",localtime(&now));
    return string(buffer);
}

void Logger::log(LogLevel level, const std::string& message) {
    lock_guard<std::mutex> lock(logMutex);
    string entry = "[" + currentTimestamp() + "] [" +levelToString(level) + "] " + message;
    cout << entry <<endl;
    if (logFile.is_open()) {
        logFile << entry <<endl;
        logFile.flush();
    }
}
