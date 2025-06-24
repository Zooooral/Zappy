/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameScreen
*/

#ifndef GAMESCREEN_HPP_
    #define GAMESCREEN_HPP_

    #include <memory>
    #include <string>
    #include <set>

    #include "../core/AGameState.hpp"
    #include "../ui/Button.hpp"
    #include "../ui/InventoryUI.hpp"
    #include "raylib.h"

class GameScreen : public AGameState {
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
    float _updateTimer = 0.0f;
    int _nextPlayerId = 1;
    std::set<int> _activePlayerIds;
    std::unique_ptr<Button> _backButton;
    std::unique_ptr<InventoryUI> _inventoryUI;

    void setupNetworkAndRequestData();
    void requestInitialGameData();
    void requestPlayerUpdates();
    void handleServerCommand(const std::string& command);
};

#endif
