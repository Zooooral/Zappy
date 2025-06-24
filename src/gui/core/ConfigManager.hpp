/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** ConfigManager
*/

#ifndef CONFIGMANAGER_HPP_
#define CONFIGMANAGER_HPP_

#include <string>
#include <unordered_map>
#include <vector>

class ConfigManager {
public:
    static ConfigManager& getInstance();

    bool loadConfig(const std::string& filePath = "config.ini");
    bool saveConfig(const std::string& filePath = "config.ini") const;

    float getVolume() const { return _volume; }
    void setVolume(float volume) { _volume = volume; }

    int getKeyBinding(const std::string& action) const;
    void setKeyBinding(const std::string& action, int key);
    
    std::vector<std::string> getKeyBindingActions() const;

    const std::string& getHost() const { return _host; }
    void setHost(const std::string& host) { _host = host; }

    int getPort() const { return _port; }
    void setPort(int port) { _port = port; }

    void resetToDefaults();

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void initializeDefaults();

    float _volume = 0.7f;
    std::unordered_map<std::string, int> _keyBindings;
    std::string _host;
    int _port = 0;
};

#endif
