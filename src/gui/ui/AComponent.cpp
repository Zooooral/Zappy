/*
** EPITECH PROJECT, 2025
** src/gui/ui/AComponent.cpp
** File description:
** AComponent
*/

#include "AComponent.hpp"

AComponent::AComponent(const Vector2 &position, const Vector2 &size) : _position(position), _size(size)
{
    updateBounds();
}

bool AComponent::isHovered() const
{
    return CheckCollisionPointRec(GetMousePosition(), _bounds);
}

bool AComponent::isClicked() const
{
    return isHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void AComponent::setPosition(const Vector2 &position)
{
    _position = position;
    updateBounds();
}

void AComponent::setSize(const Vector2 &size)
{
    _size = size;
    updateBounds();
}

Vector2 AComponent::getPosition() const
{
    return _position;
}

Vector2 AComponent::getSize() const
{
    return _size;
}

void AComponent::updateBounds()
{
    _bounds = { _position.x, _position.y, _size.x, _size.y };
}
// Updated: 2025-05-31 00:53:43
