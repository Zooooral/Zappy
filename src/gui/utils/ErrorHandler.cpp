/*
** EPITECH PROJECT, 2025
** src/gui/utils/ErrorHandler.cpp
** File description:
** Unified error handling implementation
*/

#include "ErrorHandler.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace zappy {
namespace utils {

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::setCallback(ErrorCallback callback) {
    _callback = std::move(callback);
}

void ErrorHandler::logError(ErrorLevel level, const std::string& message) {
    if (_callback) {
        _callback(level, message);
    } else {
        // Default console output
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        const char* levelStr = "";
        switch (level) {
            case ErrorLevel::INFO: levelStr = "INFO"; break;
            case ErrorLevel::WARNING: levelStr = "WARNING"; break;
            case ErrorLevel::ERROR: levelStr = "ERROR"; break;
            case ErrorLevel::CRITICAL: levelStr = "CRITICAL"; break;
        }
        
        std::cout << "[" << oss.str() << "] [" << levelStr << "] " << message << std::endl;
    }
}

void ErrorHandler::logInfo(const std::string& message) {
    logError(ErrorLevel::INFO, message);
}

void ErrorHandler::logWarning(const std::string& message) {
    logError(ErrorLevel::WARNING, message);
}

void ErrorHandler::logError(const std::string& message) {
    logError(ErrorLevel::ERROR, message);
}

void ErrorHandler::logCritical(const std::string& message) {
    logError(ErrorLevel::CRITICAL, message);
}

}
}
