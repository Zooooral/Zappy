/*
** EPITECH PROJECT, 2025
** src/gui/core/GameState.hpp
** File description:
** GameState
*/

#ifndef GAMESTATE_HPP_
    #define GAMESTATE_HPP_

class GameState {
public:
    GameState() = default;
    virtual ~GameState() = default;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual bool isFinished() const = 0;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void setTransitionAlpha(float alpha) { _transitionAlpha = alpha; }
    virtual float getTransitionAlpha() const { return _transitionAlpha; }
    virtual bool hasShaderBackground() const { return false; }

protected:
    float _transitionAlpha = 1.0f;
};

#endif /* !GAMESTATE_HPP_ */
