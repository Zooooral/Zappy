/*
** EPITECH PROJECT, 2025
** src/gui/ui/AComponent.hpp
** File description:
** AComponent
*/

#ifndef ACOMPONENT_HPP_
    #define ACOMPONENT_HPP_

#include "IComponent.hpp"

class AComponent : public IComponent {
public:
    AComponent(const Vector2 &position, const Vector2 &size);
    virtual ~AComponent() = default;

    bool isHovered() const override;
    bool isClicked() const override;

    void setPosition(const Vector2 &position) override;
    void setSize(const Vector2 &size) override;

    Vector2 getPosition() const override;
    Vector2 getSize() const override;

protected:
    Vector2 _position;
    Vector2 _size;
    Rectangle _bounds;

    void updateBounds();
};

#endif /* !ACOMPONENT_HPP_ */
