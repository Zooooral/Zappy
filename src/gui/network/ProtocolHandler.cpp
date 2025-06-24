/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ProtocolHandler
*/

#include "ProtocolHandler.hpp"
#include <sstream>
#include <vector>

ProtocolHandler::ProtocolHandler() {
    _commandHandlers["msz"] = [this](const std::string& args) { handleMapSize(args); };
    _commandHandlers["bct"] = [this](const std::string& args) { handleTileContent(args); };
    _commandHandlers["pnw"] = [this](const std::string& args) { handlePlayerNew(args); };
    _commandHandlers["ppo"] = [this](const std::string& args) { handlePlayerPosition(args); };
    _commandHandlers["plv"] = [this](const std::string& args) { handlePlayerLevel(args); };
    _commandHandlers["pin"] = [this](const std::string& args) { handlePlayerInventory(args); };
    _commandHandlers["seg"] = [this](const std::string& args) { handleGameEnd(args); };
}

void ProtocolHandler::handleCommand(const std::string& command) {
    if (command.empty()) return;
    
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    auto it = _commandHandlers.find(cmd);
    if (it != _commandHandlers.end()) {
        std::string args = command.substr(cmd.length());
        if (!args.empty() && args[0] == ' ') {
            args = args.substr(1);
        }
        it->second(args);
    }
}

bool ProtocolHandler::isValidCommand(const std::string& command) const {
    if (command.empty()) return false;
    
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    return _commandHandlers.find(cmd) != _commandHandlers.end();
}

void ProtocolHandler::handleMapSize(const std::string& args) {
    std::istringstream iss(args);
    int width, height;
    if (iss >> width >> height && _mapSizeCallback) {
        _mapSizeCallback(width, height);
    }
}

void ProtocolHandler::handleTileContent(const std::string& args) {
    std::istringstream iss(args);
    int x, y;
    std::vector<int> resources(7);
    
    if (iss >> x >> y) {
        for (int i = 0; i < 7; ++i) {
            iss >> resources[i];
        }
        if (_tileContentCallback) {
            _tileContentCallback(x, y, resources);
        }
    }
}

void ProtocolHandler::handlePlayerNew(const std::string& args) {
    if (_playerCallback) {
        _playerCallback("pnw " + args);
    }
}

void ProtocolHandler::handlePlayerPosition(const std::string& args) {
    if (_playerCallback) {
        _playerCallback("ppo " + args);
    }
}

void ProtocolHandler::handlePlayerLevel(const std::string& args) {
    if (_playerCallback) {
        _playerCallback("plv " + args);
    }
}

void ProtocolHandler::handlePlayerInventory(const std::string& args) {
    if (_playerCallback) {
        _playerCallback("pin " + args);
    }
}

void ProtocolHandler::handleGameEnd(const std::string& args) {
    if (_playerCallback) {
        _playerCallback("seg " + args);
    }
}

void ProtocolHandler::setMapSizeCallback(std::function<void(int, int)> callback) {
    _mapSizeCallback = callback;
}

void ProtocolHandler::setTileContentCallback(std::function<void(int, int, const std::vector<int>&)> callback) {
    _tileContentCallback = callback;
}

void ProtocolHandler::setPlayerCallback(std::function<void(const std::string&)> callback) {
    _playerCallback = callback;
}
