/*
** EPITECH PROJECT, 2025
** src/gui/core/GameStateManager.hpp
** File description:
** GameStateManager
*/

#ifndef GAMESTATEMANAGER_HPP_
    #define GAMESTATEMANAGER_HPP_

    #include <memory>
    #include <unordered_map>
    #include <string>

    #include "../interfaces/IGameState.hpp"

class GameStateManager {
public:
    enum class Transition {
        NONE,
        FADE
    };

    static GameStateManager &getInstance();
    void registerState(const std::string &name, std::unique_ptr<IGameState> state);
    void changeState(const std::string &name, Transition transition = Transition::NONE);
    void update(float dt);
    void draw();

    std::string getCurrentState() const;
    bool shouldQuit() const { return _shouldQuit; }
    IGameState* getStateInstance(const std::string &name) const;

private:
    GameStateManager();
    ~GameStateManager() = default;
    GameStateManager(const GameStateManager &) = delete;
    GameStateManager &operator=(const GameStateManager &) = delete;

    std::unordered_map<std::string, std::unique_ptr<IGameState>> _states;
    std::string _currentState;
    std::string _nextState;

    Transition _transitionType = Transition::NONE;
    bool _transitioning = false;
    bool _nextStateUpdating = false;
    bool _shouldQuit = false;
    float _transitionTimer = 0.0f;
    float _transitionDuration = 0.5f;

    void drawTransition();
    void completeTransition();
};

#endif /* !GAMESTATEMANAGER_HPP_ */
