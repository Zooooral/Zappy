/*
** EPITECH PROJECT, 2025
** src/gui/core/GameStateManager.cpp
** File description:
** GameStateManager with exception safety
*/

#include "GameStateManager.hpp"
#include "../screens/SplashScreen.hpp"
#include "../screens/EndScreen.hpp"
#include <stdexcept>
#include <iostream>

GameStateManager& GameStateManager::getInstance() {
    static GameStateManager instance;
    return instance;
}

GameStateManager::GameStateManager() = default;

void GameStateManager::registerState(const std::string& name, std::unique_ptr<IGameState> state) {
    if (!state) {
        throw std::invalid_argument("Cannot register null state");
    }
    
    _states[name] = std::move(state);
    
    if (_currentState.empty()) {
        _currentState = name;
        try {
            _states[_currentState]->onEnter();
        } catch (const std::exception& e) {
            std::cerr << "Error entering initial state: " << e.what() << std::endl;
            _currentState.clear();
            throw;
        }
    }
}

void GameStateManager::changeState(const std::string& nextState, Transition transition) {
    if (_states.find(nextState) == _states.end()) {
        throw std::invalid_argument("State not found: " + nextState);
    }
    
    if (_transitioning) {
        return; // Already transitioning
    }
    
    // Strong exception safety: prepare the transition before making any changes
    std::string oldState = _currentState;
    
    try {
        _transitioning = true;
        _nextState = nextState;
        _transitionType = transition;
        _transitionTimer = 0.0f;
        
        // If transition fails, we can rollback
        if (transition == Transition::NONE) {
            completeTransition();
        }
    } catch (const std::exception& e) {
        // Rollback on failure
        _transitioning = false;
        _nextState.clear();
        std::cerr << "Failed to change state: " << e.what() << std::endl;
        throw;
    }
}

void GameStateManager::completeTransition() {
    if (_currentState.empty() || _nextState.empty()) {
        _transitioning = false;
        return;
    }
    
    std::string oldState = _currentState;
    
    try {
        // Exit current state
        if (!_currentState.empty() && _states.find(_currentState) != _states.end()) {
            _states[_currentState]->onExit();
        }
        
        // Change state
        _currentState = _nextState;
        
        // Enter new state
        if (_states.find(_currentState) != _states.end()) {
            _states[_currentState]->onEnter();
        }
        
        // Complete transition
        _transitioning = false;
        _nextState.clear();
        
    } catch (const std::exception& e) {
        // Critical error handling - try to recover
        std::cerr << "Critical error during state transition: " << e.what() << std::endl;
        
        // Try to restore old state
        try {
            _currentState = oldState;
            if (!_currentState.empty() && _states.find(_currentState) != _states.end()) {
                _states[_currentState]->onEnter();
            }
        } catch (...) {
            // Complete failure - mark as error state
            _currentState.clear();
            _shouldQuit = true;
        }
        
        _transitioning = false;
        _nextState.clear();
        throw;
    }
}

void GameStateManager::update(float dt) {
    if (_transitioning) {
        _transitionTimer += dt;
        
        if (_transitionType == Transition::FADE) {
            if (_transitionTimer >= _transitionDuration) {
                try {
                    completeTransition();
                } catch (const std::exception& e) {
                    std::cerr << "Transition completion failed: " << e.what() << std::endl;
                }
            }
        }
    } else if (!_currentState.empty() && _states.find(_currentState) != _states.end()) {
        try {
            _states[_currentState]->update(dt);
            
            // Auto-transition logic with error handling
            std::string nextState;
            if (_currentState == "splash") {
                SplashScreen* splash = static_cast<SplashScreen*>(_states[_currentState].get());
                if (splash && splash->isFinished()) {
                    nextState = "main_menu";
                }
            } else if (_currentState == "end_screen") {
                EndScreen* endScreen = static_cast<EndScreen*>(_states[_currentState].get());
                if (endScreen && endScreen->isFinished()) {
                    _shouldQuit = true;
                }
            }
            
            if (!nextState.empty() && _states.find(nextState) != _states.end()) {
                changeState(nextState, Transition::FADE);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error updating state '" << _currentState << "': " << e.what() << std::endl;
        }
    }
}

void GameStateManager::draw() {
    ClearBackground(BLACK);
    
    try {
        if (_transitioning) {
            float progress = _transitionTimer / _transitionDuration;
            if (progress < 0.5f) {
                if (!_currentState.empty() && _states.find(_currentState) != _states.end()) {
                    _states[_currentState]->draw();
                }
            } else {
                if (_states.find(_nextState) != _states.end()) {
                    _states[_nextState]->draw();
                }
            }
        } else if (!_currentState.empty() && _states.find(_currentState) != _states.end()) {
            _states[_currentState]->draw();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error drawing state: " << e.what() << std::endl;
        // Continue execution - don't crash on draw errors
    }
}

std::string GameStateManager::getCurrentState() const {
    return _currentState;
}

IGameState* GameStateManager::getStateInstance(const std::string &name) const {
    auto it = _states.find(name);
    if (it != _states.end()) {
        return it->second.get();
    }
    return nullptr;
}
