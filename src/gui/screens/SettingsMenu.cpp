/*
** EPITECH PROJECT, 2025
** src/gui/screens/SettingsMenu.cpp
** File description:
** SettingsMenu
*/

#include "SettingsMenu.hpp"
#include "../core/FontManager.hpp"
#include "../core/SoundManager.hpp"
#include "../core/GameStateManager.hpp"
#include "../core/ConfigManager.hpp"

#include <iostream>

SettingsMenu::SettingsMenu()
{
    _shader = LoadShader(NULL, "assets/shaders/main_menu.glsl");
    _timeLoc = GetShaderLocation(_shader, "iTime");
    _resolutionLoc = GetShaderLocation(_shader, "iResolution");
    _alphaLoc = GetShaderLocation(_shader, "iAlpha");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

    loadSettings();
    initializeComponents();
}

SettingsMenu::~SettingsMenu()
{
    if (IsWindowReady()) {
        UnloadShader(_shader);
    }
}

void SettingsMenu::onEnter()
{
    loadSettings();
    _finished = false;
    setTransitionAlpha(1.0f);
}

void SettingsMenu::onExit()
{
    saveSettings();
}

void SettingsMenu::initializeComponents()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float volumeY = screenH * 0.35f;
    float keybindStartY = screenH * 0.45f;
    float keybindSpacing = screenH * 0.05f;
    float buttonY = screenH * 0.85f;

    _volumeSlider = std::make_unique<Slider>(
        Vector2{(float)(screenW - (350 + 130)) / 2, volumeY},
        Vector2{300 + 145, 30},
        0.0f, 1.0f, _volume
    );
    _volumeSlider->setLabel("Volume");
    _volumeSlider->setCallback([this](float value) {
        _volume = value;
        SoundManager::getInstance().setVolume(_volume);
    });

    int i = 0;

    for (const auto &pair : _keyCodes) {
        auto keyBind = std::make_unique<KeyBindButton>(
            Vector2{(float)(screenW - 500) / 2, (float)(keybindStartY + i * keybindSpacing)},
            Vector2{500, 40},
            pair.first, pair.second
        );

        keyBind->setCallback([this, action = pair.first](int keyCode) {
            _keyCodes[action] = keyCode;
        });

        _keyBinds.push_back(std::move(keyBind));
        i++;
    }

    _backButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200) / 2, buttonY},
        Vector2{200, 50},
        "Back"
    );
    _backButton->setCallback([]() {
        GameStateManager::getInstance().changeState("main_menu", GameStateManager::Transition::FADE);
    });

    _resetButton = std::make_unique<Button>(
        Vector2{50, buttonY},
        Vector2{200, 50},
        "Reset to Default"
    );
    _resetButton->setCallback([this]() {
        ConfigManager::getInstance().resetToDefaults();

        loadSettings();

        for (size_t i = 0; i < _keyBinds.size(); ++i) {
            auto it = _keyCodes.begin();
            std::advance(it, i);
            _keyBinds[i]->setKeyCode(it->second);
        }

        _volumeSlider->setValue(_volume);
        SoundManager::getInstance().setVolume(_volume);
    });
}

void SettingsMenu::loadSettings()
{
    ConfigManager &config = ConfigManager::getInstance();

    _volume = config.getVolume();

    _keyCodes.clear();
    for (const auto &action : config.getKeyBindingActions()) {
        _keyCodes[action] = config.getKeyBinding(action);
    }
}

void SettingsMenu::saveSettings()
{
    ConfigManager &config = ConfigManager::getInstance();

    config.setVolume(_volume);

    for (const auto &pair : _keyCodes) {
        config.setKeyBinding(pair.first, pair.second);
    }

    config.saveConfig();
}

void SettingsMenu::update(float dt)
{
    SoundManager::getInstance().updateMusicStream();

    float time = (float)GetTime();
    SetShaderValue(_shader, _timeLoc, &time, SHADER_UNIFORM_FLOAT);

    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(_shader, _alphaLoc, &_transitionAlpha, SHADER_UNIFORM_FLOAT);

    _volumeSlider->update(dt);

    for (auto &keyBind : _keyBinds) {
        keyBind->update(dt);
    }

    _backButton->update(dt);
    _resetButton->update(dt);
}

void SettingsMenu::draw()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BLACK);

    BeginShaderMode(_shader);
    DrawRectangle(0, 0, screenW, screenH, Fade(WHITE, _transitionAlpha));
    EndShaderMode();

    Color textColor = Fade(WHITE, _transitionAlpha);

    const char *title = "SETTINGS";
    int fontSize = 48;
    Font fontBold = FontManager::getInstance().getFont("bold");
    Vector2 titleSize = MeasureTextEx(fontBold, title, fontSize, 1);

    DrawTextEx(fontBold, title, {(float)(screenW - titleSize.x) / 2, screenH * 0.15f}, fontSize, 1, textColor);

    _volumeSlider->draw();

    for (auto &keyBind : _keyBinds) {
        keyBind->draw();
    }

    _backButton->draw();
    _resetButton->draw();
}

bool SettingsMenu::isFinished() const
{
    return _finished;
}

bool SettingsMenu::hasShaderBackground() const
{
    return true;
}
// Updated: 2025-05-31 00:53:36
