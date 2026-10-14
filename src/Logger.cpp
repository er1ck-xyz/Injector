#include "Logger.h"
#include <iomanip>
#include <sstream>

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

void Logger::AddLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);

    logs.emplace_back(GetTimestamp(), level, message);

    if (logs.size() > MAX_LOGS) {
        logs.erase(logs.begin());
    }

    std::string levelStr;
    switch (level) {
        case LogLevel::Info:    levelStr = "[INFO]"; break;
        case LogLevel::Success: levelStr = "[✓ SUCCESS]"; break;
        case LogLevel::Warning: levelStr = "[⚠ WARNING]"; break;
        case LogLevel::Error:   levelStr = "[✗ ERROR]"; break;
    }

    std::cout << "[" << GetTimestamp() << "] " << levelStr << " "
              << message << std::endl;
}

void Logger::Info(const std::string& message) {
    GetInstance().AddLog(LogLevel::Info, message);
}

void Logger::Success(const std::string& message) {
    GetInstance().AddLog(LogLevel::Success, message);
}

void Logger::Warning(const std::string& message) {
    GetInstance().AddLog(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    GetInstance().AddLog(LogLevel::Error, message);
}

std::vector<LogMessage> Logger::GetLogs() {
    std::lock_guard<std::mutex> lock(GetInstance().logMutex);
    return GetInstance().logs;
}

void Logger::Clear() {
    std::lock_guard<std::mutex> lock(GetInstance().logMutex);
    GetInstance().logs.clear();
}

size_t Logger::GetLogCount() {
    std::lock_guard<std::mutex> lock(GetInstance().logMutex);
    return GetInstance().logs.size();
}
