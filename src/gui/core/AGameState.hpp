/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** AGameState
*/

#ifndef AGAMESTATE_HPP_
#define AGAMESTATE_HPP_

#include "../interfaces/IGameState.hpp"

class AGameState : public IGameState {
public:
    AGameState() = default;
    virtual ~AGameState() = default;

    virtual bool hasShaderBackground() const { return false; }
    void setTransitionAlpha(float alpha) { _transitionAlpha = alpha; }
    float getTransitionAlpha() const { return _transitionAlpha; }

protected:
    float _transitionAlpha = 1.0f;
    bool _finished = false;
};

#endif
