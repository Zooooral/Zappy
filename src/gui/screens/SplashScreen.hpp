/*
** EPITECH PROJECT, 2025
** src/gui/screens/SplashScreen.hpp
** File description:
** SplashScreen
*/

#ifndef SPLASHSCREEN_HPP_
    #define SPLASHSCREEN_HPP_

#include "../core/GameState.hpp"
#include "raylib.h"

#include <string>
#include <vector>

class SplashScreen : public GameState {
public:
    SplashScreen();
    ~SplashScreen();

    void update(float dt) override;
    void draw() override;
    bool isFinished() const override;

    void onEnter() override;
    void onExit() override;

private:
    struct SplashText {
        std::string line1;
        std::string line2;
    };
    std::vector<SplashText> _texts;
    int _currentIndex = 0;
    float _timer = 0.0f;
    float _fade = 0.0f;
    bool _fadingIn = true;
    bool _finished = false;

    static constexpr float FADE_TIME = 1.0f;
    static constexpr float SHOW_TIME = 1.2f;
};

#endif /* !SPLASHSCREEN_HPP_ */
