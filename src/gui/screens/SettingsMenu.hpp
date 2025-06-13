/*
** EPITECH PROJECT, 2025
** src/gui/screens/SettingsMenu.hpp
** File description:
** SettingsMenu
*/

#ifndef SETTINGSMENU_HPP_
    #define SETTINGSMENU_HPP_

    #include <memory>
    #include <vector>
    #include <string>
    #include <unordered_map>

    #include "../core/GameState.hpp"
    #include "../ui/Button.hpp"
    #include "../ui/Slider.hpp"
    #include "../ui/KeyBindButton.hpp"
    #include "raylib.h"

class SettingsMenu : public GameState {
public:
    SettingsMenu();
    ~SettingsMenu();

    void update(float dt) override;
    void draw() override;
    bool isFinished() const override;
    bool hasShaderBackground() const override;

    void onEnter() override;
    void onExit() override;

private:
    Shader _shader;
    int _timeLoc;
    int _resolutionLoc;
    int _alphaLoc;

    std::unique_ptr<Slider> _volumeSlider;
    std::vector<std::unique_ptr<KeyBindButton>> _keyBinds;
    std::unique_ptr<Button> _backButton;
    std::unique_ptr<Button> _resetButton;

    float _volume = 1.0f;
    std::unordered_map<std::string, int> _keyCodes;

    bool _finished = false;

    void initializeComponents();
    void loadSettings();
    void saveSettings();
    void checkAndResetOtherListeningButtons(size_t currentIndex);
};

#endif /* !SETTINGSMENU_HPP_ */
