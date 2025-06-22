/*
** EPITECH PROJECT, 2025
** src/gui/screens/MainMenu.hpp
** File description:
** MainMenu
*/

#ifndef MAINMENU_HPP_
    #define MAINMENU_HPP_

    #include <memory>
    #include <vector>

    #include "../core/GameState.hpp"
    #include "../ui/Button.hpp"
    #include "raylib.h"

class MainMenu : public GameState {
public:
    MainMenu();
    ~MainMenu();

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

    std::vector<std::unique_ptr<Button>> _buttons;
    bool _finished = false;
    bool _quitRequested = false;

    void initializeButtons();
};

#endif /* !MAINMENU_HPP_ */
