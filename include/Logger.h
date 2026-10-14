#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <iostream>

enum class LogLevel {
    Info,
    Success,
    Warning,
    Error
};

struct LogMessage {
    std::string timestamp;
    LogLevel level;
    std::string message;

    LogMessage(const std::string& ts, LogLevel lv, const std::string& msg)
        : timestamp(ts), level(lv), message(msg) {}
};

class Logger {
private:
    static Logger& GetInstance();

    mutable std::mutex logMutex;
    std::vector<LogMessage> logs;
    static constexpr size_t MAX_LOGS = 1000;

    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:
    static void Info(const std::string& message);
    static void Success(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);

    static std::vector<LogMessage> GetLogs();
    static void Clear();
    static size_t GetLogCount();

private:
    static std::string GetTimestamp();
    void AddLog(LogLevel level, const std::string& message);
};
