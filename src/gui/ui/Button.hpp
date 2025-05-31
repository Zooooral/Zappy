/*
** EPITECH PROJECT, 2025
** src/gui/ui/Button.hpp
** File description:
** Button
*/

#ifndef BUTTON_HPP_
    #define BUTTON_HPP_

#include "AComponent.hpp"

#include <string>
#include <functional>

class Button : public AComponent {
public:
    Button(const Vector2 &position, const Vector2 &size, const std::string &text);
    ~Button() = default;

    void update(float dt) override;
    void draw() const override;

    void setCallback(const std::function<void()> &callback);
    void setText(const std::string &text);

private:
    std::string _text;
    std::function<void()> _callback;
    bool _pressed = false;
    Color _baseColor = DARKGRAY;
    Color _hoverColor = WHITE;
    Color _pressColor = GRAY;
};

#endif /* !BUTTON_HPP_ */
