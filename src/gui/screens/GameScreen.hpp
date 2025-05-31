/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameScreen
*/

#ifndef GAMESCREEN_HPP_
    #define GAMESCREEN_HPP_

#include "../core/GameState.hpp"
#include "../ui/Button.hpp"
#include "../ui/InventoryUI.hpp"
#include "raylib.h"
#include <memory>
#include <string>

class GameScreen : public GameState {
public:
    GameScreen();
    ~GameScreen();

    void update(float dt) override;
    void draw() override;
    bool isFinished() const override;
    void triggerGameEnd(const std::string& winningTeam);

    void onEnter() override;
    void onExit() override;

private:
    bool _finished = false;
    bool _shouldReturn = false;
    bool _mapInitialized = false;
    std::unique_ptr<Button> _backButton;
    std::unique_ptr<InventoryUI> _inventoryUI;
    
    void setupNetworkAndRequestData();
    void requestInitialGameData();
    void handleServerCommand(const std::string& command);
};

#endif
