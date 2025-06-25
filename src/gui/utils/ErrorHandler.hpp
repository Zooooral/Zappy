/*
** EPITECH PROJECT, 2025
** src/gui/utils/ErrorHandler.hpp
** File description:
** Unified error handling system
*/

#ifndef ERRORHANDLER_HPP_
#define ERRORHANDLER_HPP_

#include <string>
#include <functional>
#include <memory>

namespace zappy {
namespace utils {

enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class ErrorHandler {
public:
    using ErrorCallback = std::function<void(ErrorLevel, const std::string&)>;
    
    static ErrorHandler& getInstance();
    
    void setCallback(ErrorCallback callback);
    void logError(ErrorLevel level, const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void logCritical(const std::string& message);

private:
    ErrorHandler() = default;
    ErrorCallback _callback;
};

// Convenience macros
#define LOG_INFO(msg) zappy::utils::ErrorHandler::getInstance().logInfo(msg)
#define LOG_WARNING(msg) zappy::utils::ErrorHandler::getInstance().logWarning(msg)
#define LOG_ERROR(msg) zappy::utils::ErrorHandler::getInstance().logError(msg)
#define LOG_CRITICAL(msg) zappy::utils::ErrorHandler::getInstance().logCritical(msg)

}
}

#endif /* !ERRORHANDLER_HPP_ */
