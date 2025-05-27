/*
** EPITECH PROJECT, 2025
** src/gui/ui/Button.cpp
** File description:
** Button
*/

#include "../core/FontManager.hpp"
#include "Button.hpp"

Button::Button(const Vector2 &position, const Vector2 &size, const std::string &text) : AComponent(position, size), _text(text)
{

}

void Button::update(float dt)
{
    (void)dt;
    if (isHovered()) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            _pressed = true;
        } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && _pressed) {
            _pressed = false;
            if (_callback) {
                _callback();
            }
        }
    } else {
        _pressed = false;
    }
}

void Button::draw() const
{
    Color bgColor = Fade(BLACK, 0.5f);
    Color borderColor = _baseColor;

    if (_pressed) {
        borderColor = _pressColor;
    } else if (isHovered()) {
        borderColor = _hoverColor;
    }
    borderColor = Fade(borderColor, 0.75f);
    DrawRectangleRounded(_bounds, 0.3f, 10, bgColor);
    DrawRectangleRoundedLines(_bounds, 0.3f, 10, borderColor);

    Font font = FontManager::getInstance().getFont("medium");
    Vector2 textSize = MeasureTextEx(font, _text.c_str(), 24, 1);

    float textX = _position.x + (_size.x - textSize.x) / 2;
    float textY = _position.y + (_size.y - textSize.y) / 2;
    DrawTextEx(font, _text.c_str(), {textX, textY}, 24, 1, RAYWHITE);
}

void Button::setCallback(const std::function<void()> &callback)
{
    _callback = callback;
}

void Button::setText(const std::string &text)
{
    _text = text;
}
// Updated: 2025-05-31 00:53:02
