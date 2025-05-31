/*
** EPITECH PROJECT, 2025
** src/gui/core/ConfigManager.hpp
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
    static ConfigManager &getInstance();

    float getVolume() const;
    void setVolume(float volume);

    int getKeyBinding(const std::string &action) const;
    void setKeyBinding(const std::string &action, int keyCode);
    std::vector<std::string> getKeyBindingActions() const;

    std::string getHost() const;
    void setHost(const std::string &host);
    int getPort() const;
    void setPort(int port);

    bool loadConfig(const std::string &filePath = "config.ini");
    bool saveConfig(const std::string &filePath = "config.ini") const;

    void resetToDefaults();

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager &) = delete;
    ConfigManager &operator=(const ConfigManager &) = delete;

    float _volume = 1.0f;
    std::unordered_map<std::string, int> _keyBindings;
    std::string _host = "localhost";
    int _port = 4242;

    void initializeDefaults();
};

#endif /* !CONFIGMANAGER_HPP_ */
