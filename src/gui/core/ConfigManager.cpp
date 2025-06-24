/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ConfigManager
*/

#include "ConfigManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "raylib.h"

ConfigManager& ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::initializeDefaults()
{
    _volume = 0.7f;
    _keyBindings["UP"] = KEY_W;
    _keyBindings["DOWN"] = KEY_S;
    _keyBindings["LEFT"] = KEY_A;
    _keyBindings["RIGHT"] = KEY_D;
    _keyBindings["ESCAPE"] = KEY_ESCAPE;
    _keyBindings["ENTER"] = KEY_ENTER;
    _keyBindings["SPACE"] = KEY_SPACE;
    
    _host = "";
    _port = 0;
}

int ConfigManager::getKeyBinding(const std::string& action) const
{
    auto it = _keyBindings.find(action);
    if (it != _keyBindings.end()) {
        return it->second;
    }
    
    if (action == "UP") return KEY_W;
    if (action == "DOWN") return KEY_S;
    if (action == "LEFT") return KEY_A;
    if (action == "RIGHT") return KEY_D;
    if (action == "ESCAPE") return KEY_ESCAPE;
    if (action == "ENTER") return KEY_ENTER;
    if (action == "SPACE") return KEY_SPACE;
    
    return KEY_NULL;
}

void ConfigManager::setKeyBinding(const std::string& action, int key)
{
    _keyBindings[action] = key;
}

std::vector<std::string> ConfigManager::getKeyBindingActions() const
{
    std::vector<std::string> actions;
    for (const auto& pair : _keyBindings) {
        actions.push_back(pair.first);
    }
    
    if (_keyBindings.find("UP") == _keyBindings.end()) actions.push_back("UP");
    if (_keyBindings.find("DOWN") == _keyBindings.end()) actions.push_back("DOWN");
    if (_keyBindings.find("LEFT") == _keyBindings.end()) actions.push_back("LEFT");
    if (_keyBindings.find("RIGHT") == _keyBindings.end()) actions.push_back("RIGHT");
    if (_keyBindings.find("ESCAPE") == _keyBindings.end()) actions.push_back("ESCAPE");
    if (_keyBindings.find("ENTER") == _keyBindings.end()) actions.push_back("ENTER");
    if (_keyBindings.find("SPACE") == _keyBindings.end()) actions.push_back("SPACE");
    
    return actions;
}

bool ConfigManager::loadConfig(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        initializeDefaults();
        return false;
    }

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
            continue;
        }

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (section == "Game") {
                if (key == "Volume") {
                    _volume = std::stof(value);
                }
            } else if (section == "KeyBindings") {
                _keyBindings[key] = std::stoi(value);
            } else if (section == "Network") {
                if (key == "Host") {
                    _host = value;
                } else if (key == "Port") {
                    _port = std::stoi(value);
                }
            }
        }
    }

    file.close();
    return true;
}

bool ConfigManager::saveConfig(const std::string& filePath) const
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    file << "[Game]" << std::endl;
    file << "Volume=" << _volume << std::endl;
    file << std::endl;

    file << "[KeyBindings]" << std::endl;
    for (const auto& pair : _keyBindings) {
        file << pair.first << "=" << pair.second << std::endl;
    }
    file << std::endl;

    file << "[Network]" << std::endl;
    file << "Host=" << _host << std::endl;
    file << "Port=" << _port << std::endl;

    file.close();
    return true;
}

void ConfigManager::resetToDefaults()
{
    initializeDefaults();
}
