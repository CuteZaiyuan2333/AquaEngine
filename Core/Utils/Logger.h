#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace Aqua {
namespace Utils {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
};

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void SetLogLevel(LogLevel level) { m_logLevel = level; }
    LogLevel GetLogLevel() const { return m_logLevel; }

    template<typename... Args>
    void Log(LogLevel level, const std::string& category, Args&&... args) {
        if (level < m_logLevel) return;

        std::ostringstream oss;
        oss << GetTimestamp() << " [" << LogLevelToString(level) << "] [" << category << "] ";
        LogImpl(oss, std::forward<Args>(args)...);
        
        auto message = oss.str();
        if (level >= LogLevel::Error) {
            std::cerr << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }

private:
    LogLevel m_logLevel = LogLevel::Info;

    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    std::string LogLevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO ";
            case LogLevel::Warning: return "WARN ";
            case LogLevel::Error:   return "ERROR";
            default:                return "UNKNW";
        }
    }

    template<typename T>
    void LogImpl(std::ostringstream& oss, T&& arg) {
        oss << std::forward<T>(arg);
    }

    template<typename T, typename... Args>
    void LogImpl(std::ostringstream& oss, T&& first, Args&&... rest) {
        oss << std::forward<T>(first);
        LogImpl(oss, std::forward<Args>(rest)...);
    }
};

// 便捷宏定义
#define AQUA_LOG_DEBUG(category, ...) \
    Aqua::Utils::Logger::GetInstance().Log(Aqua::Utils::LogLevel::Debug, category, __VA_ARGS__)

#define AQUA_LOG_INFO(category, ...) \
    Aqua::Utils::Logger::GetInstance().Log(Aqua::Utils::LogLevel::Info, category, __VA_ARGS__)

#define AQUA_LOG_WARNING(category, ...) \
    Aqua::Utils::Logger::GetInstance().Log(Aqua::Utils::LogLevel::Warning, category, __VA_ARGS__)

#define AQUA_LOG_ERROR(category, ...) \
    Aqua::Utils::Logger::GetInstance().Log(Aqua::Utils::LogLevel::Error, category, __VA_ARGS__)

} // namespace Utils
} // namespace Aqua