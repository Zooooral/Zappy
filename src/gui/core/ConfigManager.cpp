/*
** EPITECH PROJECT, 2025
** src/gui/core/ConfigManager.cpp
** File description:
** ConfigManager
*/

#include "ConfigManager.hpp"

#include "raylib.h"
#include <fstream>
#include <iostream>
#include <sstream>

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager()
{
    initializeDefaults();
    loadConfig();
}

void ConfigManager::initializeDefaults()
{
    _volume = 1.0f;
    _keyBindings["Move Forward"] = KEY_Z;
    _keyBindings["Move Backward"] = KEY_S;
    _keyBindings["Move Left"] = KEY_Q;
    _keyBindings["Move Right"] = KEY_D;
    _keyBindings["Toggle View"] = KEY_P;
    _host = "localhost";
    _port = 4242;
}

float ConfigManager::getVolume() const
{
    return _volume;
}

void ConfigManager::setVolume(float volume)
{
    _volume = volume;
}

int ConfigManager::getKeyBinding(const std::string &action) const
{
    auto it = _keyBindings.find(action);
    if (it != _keyBindings.end()) {
        return it->second;
    }
    return 0;
}

void ConfigManager::setKeyBinding(const std::string &action, int keyCode)
{
    _keyBindings[action] = keyCode;
}

std::vector<std::string> ConfigManager::getKeyBindingActions() const
{
    std::vector<std::string> actions;
    for (const auto &pair : _keyBindings) {
        actions.push_back(pair.first);
    }
    return actions;
}

std::string ConfigManager::getHost() const
{
    return _host;
}

void ConfigManager::setHost(const std::string &host)
{
    _host = host;
}

int ConfigManager::getPort() const
{
    return _port;
}

void ConfigManager::setPort(int port)
{
    _port = port;
}

bool ConfigManager::loadConfig(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        if (line[0] == '[' && line[line.size() - 1] == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        size_t delimPos = line.find('=');
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);

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

bool ConfigManager::saveConfig(const std::string &filePath) const
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open config file for writing: " << filePath << std::endl;
        return false;
    }

    file << "[Game]" << std::endl;
    file << "Volume=" << _volume << std::endl;
    file << std::endl;

    file << "[KeyBindings]" << std::endl;
    for (const auto &pair : _keyBindings) {
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
