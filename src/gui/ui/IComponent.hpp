/*
** EPITECH PROJECT, 2025
** src/gui/ui/IComponent.hpp
** File description:
** IComponent interface
*/

#ifndef ICOMPONENT_HPP_
    #define ICOMPONENT_HPP_

#include "raylib.h"

class IComponent {
public:
    virtual ~IComponent() = default;

    virtual void update(float dt) = 0;
    virtual void draw() const = 0;

    virtual bool isHovered() const = 0;
    virtual bool isClicked() const = 0;

    virtual void setPosition(const Vector2 &position) = 0;
    virtual void setSize(const Vector2 &size) = 0;

    virtual Vector2 getPosition() const = 0;
    virtual Vector2 getSize() const = 0;
};

#endif /* !ICOMPONENT_HPP_ */
