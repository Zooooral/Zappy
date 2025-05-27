/*
** EPITECH PROJECT, 2025
** src/gui/core/GameStateManager.cpp
** File description:
** GameStateManager
*/

#include "GameStateManager.hpp"
#include "../screens/EndScreen.hpp"
#include "raylib.h"

#include <iostream>

GameStateManager &GameStateManager::getInstance()
{
    static GameStateManager instance;
    return instance;
}

GameStateManager::GameStateManager()
{
    _shouldQuit = false;
}

void GameStateManager::registerState(const std::string &name, std::unique_ptr<GameState> state)
{
    _states[name] = std::move(state);
    if (_currentState.empty()) {
        _currentState = name;
        _states[_currentState]->onEnter();
    }
}

void GameStateManager::changeState(const std::string &name, Transition transition)
{
    if (_states.find(name) == _states.end()) {
        std::cerr << "Warning: State '" << name << "' not found" << std::endl;
        return;
    }

    if (name == _currentState)
        return;

    if (_transitioning && _nextState == name) {
        return;
    }

    if (transition == Transition::NONE) {
        if (!_currentState.empty()) {
            _states[_currentState]->onExit();
        }
        _currentState = name;
        _states[_currentState]->onEnter();
    } else {
        _transitionType = transition;
        _transitioning = true;
        _transitionTimer = 0.0f;
        _nextState = name;
    }
}

void GameStateManager::update(float dt)
{
    if (_transitioning) {
        _transitionTimer += dt;
        float progress = _transitionTimer / _transitionDuration;
        if (progress >= 1.0f) {
            _transitioning = false;
            if (!_currentState.empty()) {
                _states[_currentState]->onExit();
            }
            _currentState = _nextState;
            _nextState.clear();
            _states[_currentState]->onEnter();
            _states[_currentState]->setTransitionAlpha(1.0f);
        } else {
            if (progress < 0.5f) {
                float fadeOut = 1.0f - (progress * 2.0f);
                if (!_currentState.empty()) {
                    _states[_currentState]->setTransitionAlpha(fadeOut);
                }
            } else {
                float fadeIn = (progress - 0.5f) * 2.0f;
                if (_states.find(_nextState) != _states.end()) {
                    _states[_nextState]->setTransitionAlpha(fadeIn);
                }
            }
        }
        if (progress < 0.5f && !_currentState.empty()) {
            _states[_currentState]->update(dt);
        } else if (progress >= 0.5f && _states.find(_nextState) != _states.end()) {
            if (!_nextStateUpdating) {
                _nextStateUpdating = true;
            }
            _states[_nextState]->update(dt);
        }
    } else if (!_currentState.empty()) {
        _states[_currentState]->update(dt);
        if (_states[_currentState]->isFinished()) {
            std::string nextState = "";
            if (_currentState == "splash") {
                nextState = "main_menu";
            } else if (_currentState == "main_menu" && _states.find("game") != _states.end()) {
            }
            else if (_currentState == "end_screen") {
                EndScreen* endScreen = static_cast<EndScreen*>(_states[_currentState].get());
                if (endScreen && endScreen->isFinished()) {
                    _shouldQuit = true;
                }
            }
            if (!nextState.empty() && _states.find(nextState) != _states.end()) {
                changeState(nextState, Transition::FADE);
            }
        }
    }
}

void GameStateManager::draw()
{
    ClearBackground(BLACK);
    if (_transitioning) {
        float progress = _transitionTimer / _transitionDuration;
        if (progress < 0.5f) {
            if (!_currentState.empty()) {
                _states[_currentState]->draw();
            }
        } else {
            if (_states.find(_nextState) != _states.end()) {
                _states[_nextState]->draw();
            }
        }
    } else if (!_currentState.empty()) {
        _states[_currentState]->draw();
    }
}

void GameStateManager::drawTransition() {}

std::string GameStateManager::getCurrentState() const
{
    return _currentState;
}

GameState* GameStateManager::getStateInstance(const std::string &name)
{
    auto it = _states.find(name);
    if (it != _states.end()) {
        return it->second.get();
    }
    return nullptr;
}
// Updated: 2025-05-31 00:53:04
