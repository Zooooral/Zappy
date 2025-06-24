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
#include <algorithm>
#include "raylib.h"

ConfigManager& ConfigManager::getInstance()
{
    static ConfigManager instance;
    static bool initialized = false;
    if (!initialized) {
        instance.initializeDefaults();
        instance.loadConfig();
        initialized = true;
    }
    return instance;
}

void ConfigManager::initializeDefaults()
{
    _volume = 0.7f;
    
    _keyBindings["Move Forward"] = KEY_W;
    _keyBindings["Move Backward"] = KEY_S;
    _keyBindings["Move Left"] = KEY_A;
    _keyBindings["Move Right"] = KEY_D;
    _keyBindings["Toggle View"] = KEY_P;
    
    _host = "localhost";
    _port = 4242;
}

int ConfigManager::getKeyBinding(const std::string& action) const
{
    auto it = _keyBindings.find(action);
    if (it != _keyBindings.end()) {
        return it->second;
    }
    
    if (action == "Move Forward") return KEY_W;
    if (action == "Move Backward") return KEY_S;
    if (action == "Move Left") return KEY_A;
    if (action == "Move Right") return KEY_D;
    if (action == "Toggle View") return KEY_P;
    
    return KEY_NULL;
}

void ConfigManager::setKeyBinding(const std::string& action, int key)
{
    _keyBindings[action] = key;
}

std::vector<std::string> ConfigManager::getKeyBindingActions() const
{
    std::vector<std::string> actions = {
        "Move Forward", "Move Backward", "Move Left", "Move Right", 
        "Toggle View"
    };
    
    for (const auto& pair : _keyBindings) {
        if (std::find(actions.begin(), actions.end(), pair.first) == actions.end()) {
            actions.push_back(pair.first);
        }
    }
    
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
