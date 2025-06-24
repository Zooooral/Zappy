/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** IProtocolHandler
*/

#ifndef IPROTOCOLHANDLER_HPP_
#define IPROTOCOLHANDLER_HPP_

#include <string>

class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    virtual void handleCommand(const std::string& command) = 0;
    virtual bool isValidCommand(const std::string& command) const = 0;
};

#endif
