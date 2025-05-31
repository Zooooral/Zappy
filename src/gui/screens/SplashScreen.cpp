/*
** EPITECH PROJECT, 2025
** src/gui/screens/SplashScreen.cpp
** File description:
** SplashScreen
*/

#include "SplashScreen.hpp"
#include "../core/FontManager.hpp"
#include "../core/SoundManager.hpp"
#include "../core/GameStateManager.hpp"

#include <iostream>
#include <cmath>

SplashScreen::SplashScreen()
{
    _texts = {
        {"THE TRYADE COLLECTIVE", "PRESENTS"},
        {"IN COLLABORATION WITH", "EPITECH"},
        {"THE 2025 EDITION OF", "ZAPPY"}
    };
}

SplashScreen::~SplashScreen()
{

}

void SplashScreen::onEnter()
{
    SoundManager::getInstance().playMusic("assets/sounds/soundtrack.ogg", true);
    _currentIndex = 0;
    _timer = 0.0f;
    _fadingIn = true;
    _fade = 0.0f;
    _finished = false;
}

void SplashScreen::onExit()
{

}

void SplashScreen::update(float dt)
{
    if (_finished)
        return;
    if (IsKeyPressed(KEY_SPACE)) {
        _finished = true;
        return;
    }

    SoundManager::getInstance().updateMusicStream();
    _timer += dt;

    if (_fadingIn) {
        _fade = _timer / FADE_TIME;
        if (_timer >= FADE_TIME) {
            _timer = 0.0f;
            _fade = 1.0f;
            _fadingIn = false;
        }
    } else {
        if (_timer >= SHOW_TIME) {
            _fade = 1.0f - ((_timer - SHOW_TIME) / FADE_TIME);
            if ((_timer - SHOW_TIME) >= FADE_TIME) {
                _currentIndex++;
                _timer = 0.0f;
                _fadingIn = true;
                _fade = 0.0f;
                if (_currentIndex >= (int)_texts.size()) {
                    _finished = true;
                }
            }
        } else {
            _fade = 1.0f;
        }
    }
}

void SplashScreen::draw()
{
    if (_finished || _currentIndex >= (int)_texts.size())
        return;

    const SplashText &txt = _texts[_currentIndex];
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float alpha = _fade;
    int fontSizeThin = 32;
    int fontSizeBold = 44;

    ClearBackground(BLACK);

    Font fontThin = FontManager::getInstance().getFont("thin");
    Font fontBold = FontManager::getInstance().getFont("bold");
    Font fontMedium = FontManager::getInstance().getFont("medium");

    Vector2 size1 = MeasureTextEx(fontThin, txt.line1.c_str(), fontSizeThin, 1);
    Vector2 size2 = MeasureTextEx(fontBold, txt.line2.c_str(), fontSizeBold, 1);
    int y = screenH / 2 - (size1.y + size2.y + 10) / 2;
    float x1 = std::floor((screenW - size1.x) / 2);
    float y1 = std::floor((float)y);
    float x2 = std::floor((screenW - size2.x) / 2);
    float y2 = std::floor((float)(y + size1.y + 10));
    DrawTextEx(fontThin, txt.line1.c_str(), {x1, y1}, fontSizeThin, 1, Fade(WHITE, alpha));
    DrawTextEx(fontBold, txt.line2.c_str(), {x2, y2}, fontSizeBold, 1, Fade(WHITE, alpha));
    const char* skipText = "PRESS SPACE TO SKIP";
    int skipFontSize = 24;
    Vector2 skipSize = MeasureTextEx(fontMedium, skipText, skipFontSize, 1);
    float skipX = std::floor((screenW - skipSize.x) / 2);
    float skipY = std::floor(screenH - skipSize.y - 40);
    DrawTextEx(fontMedium, skipText, {skipX, skipY}, skipFontSize, 1, Fade(WHITE, alpha));
}

bool SplashScreen::isFinished() const
{
    return _finished;
}
// Updated: 2025-05-31 00:53:29
