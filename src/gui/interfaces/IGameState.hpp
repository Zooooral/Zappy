/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** IGameState
*/

#ifndef IGAMESTATE_HPP_
#define IGAMESTATE_HPP_

class IGameState {
public:
    virtual ~IGameState() = default;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual bool isFinished() const = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};

#endif
