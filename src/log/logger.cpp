#include "logger.hpp"

#include <iostream>
#include <ctime>
#include <Windows.h>

std::vector<LogEntry> Logger::messages;

std::string dateTimeToString()
{
    time_t timestamp;
    time(&timestamp);
    std::string timeStr = ctime(&timestamp);
    timeStr.pop_back();

    return timeStr;
}

void Logger::log(const std::string& message)
{
    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message = "LOG | " + dateTimeToString() + " - " + message + "\n";

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, logEntry.type);

    std::cout <<  logEntry.message;
    messages.push_back(logEntry);
}

void Logger::warning(const std::string& message)
{
    LogEntry logEntry;
    logEntry.type = LOG_WARNING;
    logEntry.message = "WAR | " + dateTimeToString() + " - " + message + "\n";

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, logEntry.type);

    std::cout <<  logEntry.message;
    messages.push_back(logEntry);
}

void Logger::err(const std::string& message)
{
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.message = "ERR | " + dateTimeToString() + " - " + message + "\n";

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, logEntry.type);

    std::cout <<  logEntry.message;
    messages.push_back(logEntry);
}