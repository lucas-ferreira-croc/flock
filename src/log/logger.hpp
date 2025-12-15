#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <vector>

enum LogType
{
    LOG_INFO = 2,
    LOG_WARNING = 6,
    LOG_ERROR = 4    
};

struct LogEntry
{
    LogType type;
    std::string message;
};

class Logger
{
public:
    static std::vector<LogEntry> messages;
    
    static void log(const std::string& message);
    static void warning(const std::string& message);
    static void err(const std::string& message);

private:
};

#endif