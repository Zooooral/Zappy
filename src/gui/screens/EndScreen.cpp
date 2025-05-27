/*
** EPITECH PROJECT, 2025
** src/gui/screens/EndScreen.cpp
** File description:
** EndScreen
*/

#include "EndScreen.hpp"
#include "../core/FontManager.hpp"
#include "../core/SoundManager.hpp"
#include "../core/GameStateManager.hpp"

#include <iostream>
#include <cmath>

EndScreen::EndScreen()
{
    _shader = LoadShader(NULL, "assets/shaders/main_menu.glsl");
    _timeLoc = GetShaderLocation(_shader, "iTime");
    _resolutionLoc = GetShaderLocation(_shader, "iResolution");
    _alphaLoc = GetShaderLocation(_shader, "iAlpha");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

    initializeComponents();
}

EndScreen::~EndScreen()
{
    if (IsWindowReady()) {
        UnloadShader(_shader);
    }
}

void EndScreen::onEnter()
{
    _finished = false;
    _exitRequested = false;
    _celebrationTimer = 0.0f;
    setTransitionAlpha(1.0f);
    
    SoundManager::getInstance().stopMusic();
}

void EndScreen::onExit()
{
    if (_exitRequested) {
        CloseWindow();
    }
}

void EndScreen::initializeComponents()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    _exitButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200) / 2, (float)(screenH * 0.8f)},
        Vector2{200, 60},
        "Exit Game"
    );
    _exitButton->setCallback([this]() {
        _exitRequested = true;
        _finished = true;
    });
}

void EndScreen::setWinningTeam(const std::string& teamName)
{
    _winningTeam = teamName;
}

void EndScreen::update(float dt)
{
    _celebrationTimer += dt;

    float time = (float)GetTime();
    SetShaderValue(_shader, _timeLoc, &time, SHADER_UNIFORM_FLOAT);

    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(_shader, _alphaLoc, &_transitionAlpha, SHADER_UNIFORM_FLOAT);

    _exitButton->update(dt);

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
        _exitRequested = true;
        _finished = true;
    }
}

void EndScreen::draw()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BLACK);

    BeginShaderMode(_shader);
    DrawRectangle(0, 0, screenW, screenH, Fade(WHITE, _transitionAlpha));
    EndShaderMode();

    Color textColor = Fade(WHITE, _transitionAlpha);
    float pulse = (sinf(_celebrationTimer * 3.0f) + 1.0f) * 0.5f;
    Color celebrationColor = ColorLerp(GOLD, YELLOW, pulse);

    Font fontBold = FontManager::getInstance().getFont("bold");
    Font fontMedium = FontManager::getInstance().getFont("medium");

    const char* gameOverText = "GAME OVER";
    int gameOverFontSize = 72;
    Vector2 gameOverSize = MeasureTextEx(fontBold, gameOverText, gameOverFontSize, 1);
    DrawTextEx(fontBold, gameOverText, 
               {(float)(screenW - gameOverSize.x) / 2, screenH * 0.2f}, 
               gameOverFontSize, 1, textColor);

    std::string winText = _winningTeam + " Won the game!";
    int winFontSize = 48;
    Vector2 winSize = MeasureTextEx(fontBold, winText.c_str(), winFontSize, 1);
    DrawTextEx(fontBold, winText.c_str(),
               {(float)(screenW - winSize.x) / 2, screenH * 0.4f},
               winFontSize, 1, Fade(celebrationColor, _transitionAlpha));

    for (int i = 0; i < 20; i++) {
        float angle = (_celebrationTimer * 2.0f + i * 18.0f) * DEG2RAD;
        float radius = 100.0f + sinf(_celebrationTimer * 1.5f + i) * 20.0f;
        float x = screenW * 0.5f + cosf(angle) * radius;
        float y = screenH * 0.4f + sinf(angle) * radius * 0.3f;
        float size = 3.0f + sinf(_celebrationTimer * 4.0f + i) * 2.0f;
        Color starColor = Fade(celebrationColor, _transitionAlpha * 0.8f);
        DrawPoly({x, y}, 5, size, _celebrationTimer * 50.0f + i * 36.0f, starColor);
    }

    const char* instructionText = "Press SPACE or ESCAPE to exit";
    int instructionFontSize = 24;
    Vector2 instructionSize = MeasureTextEx(fontMedium, instructionText, instructionFontSize, 1);
    DrawTextEx(fontMedium, instructionText,
               {(float)(screenW - instructionSize.x) / 2, screenH * 0.9f},
               instructionFontSize, 1, Fade(LIGHTGRAY, _transitionAlpha));

    _exitButton->draw();
}

bool EndScreen::isFinished() const
{
    return _finished;
}

bool EndScreen::hasShaderBackground() const
{
    return true;
}
