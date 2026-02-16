#pragma once
#include <string>
#include <fstream>
#include <mutex>
using namespace std;
enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};
class Logger {
private:
    std::ofstream logFile;
    std::mutex logMutex;
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    string levelToString(LogLevel level);
    string currentTimestamp();

public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message);
};
