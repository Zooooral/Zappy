/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ProtocolHandler
*/

#ifndef PROTOCOLHANDLER_HPP_
#define PROTOCOLHANDLER_HPP_

#include "../interfaces/IProtocolHandler.hpp"
#include <map>
#include <functional>

class ProtocolHandler : public IProtocolHandler {
public:
    ProtocolHandler();
    ~ProtocolHandler() = default;

    void handleCommand(const std::string& command) override;
    bool isValidCommand(const std::string& command) const override;

    void setMapSizeCallback(std::function<void(int, int)> callback);
    void setTileContentCallback(std::function<void(int, int, const std::vector<int>&)> callback);
    void setPlayerCallback(std::function<void(const std::string&)> callback);

private:
    std::map<std::string, std::function<void(const std::string&)>> _commandHandlers;
    
    void handleMapSize(const std::string& args);
    void handleTileContent(const std::string& args);
    void handlePlayerNew(const std::string& args);
    void handlePlayerPosition(const std::string& args);
    void handlePlayerLevel(const std::string& args);
    void handlePlayerInventory(const std::string& args);
    void handleGameEnd(const std::string& args);

    std::function<void(int, int)> _mapSizeCallback;
    std::function<void(int, int, const std::vector<int>&)> _tileContentCallback;
    std::function<void(const std::string&)> _playerCallback;
};

#endif
