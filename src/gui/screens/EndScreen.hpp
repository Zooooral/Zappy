/*
** EPITECH PROJECT, 2025
** src/gui/screens/EndScreen.hpp
** File description:
** EndScreen
*/

#ifndef ENDSCREEN_HPP_
    #define ENDSCREEN_HPP_

#include "../core/AGameState.hpp"
#include "../ui/Button.hpp"
#include "raylib.h"

#include <memory>
#include <string>

class EndScreen : public AGameState {
public:
    EndScreen();
    ~EndScreen();

    void update(float dt) override;
    void draw() override;
    bool isFinished() const override;
    bool hasShaderBackground() const override;

    void onEnter() override;
    void onExit() override;

    void setWinningTeam(const std::string& teamName);

private:
    Shader _shader;
    int _timeLoc;
    int _resolutionLoc;
    int _alphaLoc;

    std::unique_ptr<Button> _exitButton;
    std::string _winningTeam;
    bool _finished = false;
    bool _exitRequested = false;
    float _celebrationTimer = 0.0f;

    void initializeComponents();
};

#endif /* !ENDSCREEN_HPP_ */
