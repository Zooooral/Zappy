/*
** EPITECH PROJECT, 2025
** src/gui/screens/MainMenu.cpp
** File description:
** MainMenu
*/

#include <iostream>

#include "MainMenu.hpp"
#include "../core/FontManager.hpp"
#include "../core/SoundManager.hpp"
#include "../core/GameStateManager.hpp"

MainMenu::MainMenu()
{
    _shader = LoadShader(NULL, "assets/shaders/main_menu.glsl");
    _timeLoc = GetShaderLocation(_shader, "iTime");
    _resolutionLoc = GetShaderLocation(_shader, "iResolution");
    _alphaLoc = GetShaderLocation(_shader, "iAlpha");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    initializeButtons();
}

MainMenu::~MainMenu()
{
    if (IsWindowReady()) {
        UnloadShader(_shader);
    }
}

void MainMenu::onEnter()
{
    if (!SoundManager::getInstance().isMusicPlaying()) {
        SoundManager::getInstance().playMusic("assets/sounds/soundtrack.ogg", true);
    }
    _finished = false;
    setTransitionAlpha(1.0f);
}

void MainMenu::onExit()
{

}

void MainMenu::initializeButtons()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int menuY = screenH / 2;
    int menuSpacing = 70;

    auto playButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200) / 2, (float)menuY},
        Vector2{200, 50},
        "Play"
    );
    playButton->setCallback([]() {
        GameStateManager::getInstance().changeState("connecting", GameStateManager::Transition::FADE);
    });
    _buttons.push_back(std::move(playButton));

    auto settingsButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200) / 2, (float)(menuY + menuSpacing)},
        Vector2{200, 50},
        "Settings"
    );
    settingsButton->setCallback([]() {
        GameStateManager::getInstance().changeState("settings", GameStateManager::Transition::FADE);
    });
    _buttons.push_back(std::move(settingsButton));

    auto quitButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200) / 2, (float)(menuY + menuSpacing * 2)},
        Vector2{200, 50},
        "Quit"
    );
    quitButton->setCallback([this]() {
        SoundManager::getInstance().stopMusic();
        _quitRequested = true;
    });
    _buttons.push_back(std::move(quitButton));
}

void MainMenu::update(float dt)
{
    SoundManager::getInstance().updateMusicStream();

    float time = (float)GetTime();
    SetShaderValue(_shader, _timeLoc, &time, SHADER_UNIFORM_FLOAT);

    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(_shader, _alphaLoc, &_transitionAlpha, SHADER_UNIFORM_FLOAT);

    for (auto &button : _buttons) {
        button->update(dt);
    }

    if (_quitRequested) {
        _finished = true;
    }
}

void MainMenu::draw()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BLACK);
    BeginShaderMode(_shader);
    DrawRectangle(0, 0, screenW, screenH, Fade(WHITE, _transitionAlpha));
    EndShaderMode();

    Color textColor = Fade(WHITE, _transitionAlpha);

    const char *title = "ZAPPY";
    int fontSize = 64;
    Font fontBold = FontManager::getInstance().getFont("bold");
    Vector2 titleSize = MeasureTextEx(fontBold, title, fontSize, 1);
    DrawTextEx(fontBold, title, {(float)(screenW - titleSize.x) / 2, (float)screenH / 6}, fontSize, 1, textColor);

    const char *subtitle = "A TRIBUTE TO ZAPHOD BEEBLEBROX";
    int subtitleSize = 24;
    Font fontMedium = FontManager::getInstance().getFont("medium");
    Vector2 subtitleTextSize = MeasureTextEx(fontMedium, subtitle, subtitleSize, 1);
    DrawTextEx(fontMedium, subtitle, {(float)(screenW - subtitleTextSize.x) / 2, (float)screenH / 6 + fontSize + 10}, subtitleSize, 1, textColor);

    for (auto &button : _buttons) {
        button->draw();
    }
}

bool MainMenu::isFinished() const
{
    return _finished;
}

bool MainMenu::hasShaderBackground() const
{
    return true;
}
// Updated: 2025-05-31 00:53:38
