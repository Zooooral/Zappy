/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** AComponent
*/

#ifndef ACOMPONENT_HPP_
#define ACOMPONENT_HPP_

#include "raylib.h"

class AComponent {
public:
    AComponent(const Vector2& position, const Vector2& size)
        : _position(position), _size(size) {
        updateBounds();
    }
    virtual ~AComponent() = default;

    AComponent(const AComponent&) = delete;
    AComponent& operator=(const AComponent&) = delete;
    AComponent(AComponent&&) = delete;
    AComponent& operator=(AComponent&&) = delete;

    virtual void update(float dt) = 0;
    virtual void draw() const = 0;

    Vector2 getPosition() const { return _position; }
    Vector2 getSize() const { return _size; }
    void setPosition(const Vector2& position) { 
        _position = position; 
        updateBounds();
    }
    void setSize(const Vector2& size) { 
        _size = size; 
        updateBounds();
    }

    bool isHovered() const {
        return CheckCollisionPointRec(GetMousePosition(), _bounds);
    }

    bool isClicked() const {
        return isHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
    
    bool isEnabled() const { return _enabled; }
    bool isVisible() const { return _visible; }
    void setEnabled(bool enabled) { _enabled = enabled; }
    void setVisible(bool visible) { _visible = visible; }

protected:
    Vector2 _position;
    Vector2 _size;
    Rectangle _bounds;
    bool _enabled = true;
    bool _visible = true;

    void updateBounds() {
        _bounds = { _position.x, _position.y, _size.x, _size.y };
    }
};

#endif /* !ACOMPONENT_HPP_ */
