#ifndef ACOMPONENT_HPP_
#define ACOMPONENT_HPP_

#include "raylib.h"

class AComponent {
public:
    AComponent(const Vector2& position, const Vector2& size)
        : _position(position), _size(size), _visible(true), _enabled(true) {}
    
    virtual ~AComponent() = default;
    
    virtual void update(float dt) = 0;
    virtual void draw() const = 0;
    
    bool isVisible() const { return _visible; }
    bool isEnabled() const { return _enabled; }
    void setVisible(bool visible) { _visible = visible; }
    void setEnabled(bool enabled) { _enabled = enabled; }
    
    Vector2 getPosition() const { return _position; }
    Vector2 getSize() const { return _size; }
    void setPosition(const Vector2& position) { _position = position; }
    void setSize(const Vector2& size) { _size = size; }
    
    Rectangle getBounds() const { 
        return Rectangle{_position.x, _position.y, _size.x, _size.y}; 
    }
    
    bool isHovered() const {
        Vector2 mousePos = GetMousePosition();
        Rectangle bounds = getBounds();
        return CheckCollisionPointRec(mousePos, bounds);
    }
    
    bool isClicked() const {
        return isHovered() && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    }

protected:
    Vector2 _position;
    Vector2 _size;
    bool _visible;
    bool _enabled;
};

#endif
