/*
** EPITECH PROJECT, 2025
** src/gui/ui/Slider.cpp
** File description:
** Slider
*/

#include "Slider.hpp"
#include "../core/FontManager.hpp"

#include <cmath>
#include <algorithm>

Slider::Slider(const Vector2 &position, const Vector2 &size, float min, float max, float value) : AComponent(position, size), _min(min), _max(max), _value(value)
{
    _originalWidth = size.x;
    _size.x += 145;
    _value = std::max(_min, std::min(_max, _value));
}

void Slider::update(float dt)
{
    (void)dt;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHovered()) {
        _dragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        _dragging = false;
    }

    if (_dragging) {
        float mouseX = GetMouseX();
        float sliderStart = _position.x;
        float sliderEnd = _position.x + _originalWidth;
        float t = (mouseX - sliderStart) / (sliderEnd - sliderStart);
        t = std::max(0.0f, std::min(1.0f, t));
        float newValue = _min + t * (_max - _min);
        if (newValue != _value) {
            _value = newValue;
            if (_callback) {
                _callback(_value);
            }
        }
    }
}

void Slider::draw() const
{
    if (!_label.empty()) {
        Font font = FontManager::getInstance().getFont("medium");
        DrawTextEx(font, _label.c_str(), {_position.x, _position.y - 18}, 20, 1, WHITE);
    }

    Rectangle barRect = getSliderBar();
    DrawRectangleRounded({barRect.x + 2, barRect.y + 4, barRect.width, barRect.height}, 0.3f, 10, Fade(BLACK, 0.15f));
    Color barBgColor = LIGHTGRAY;
    DrawRectangleRounded(barRect, 0.3f, 10, barBgColor);

    Color fillColor = SKYBLUE;
    float fillWidth = ((_value - _min) / (_max - _min)) * barRect.width;
    Rectangle fillRect = { barRect.x, barRect.y, fillWidth, barRect.height };
    DrawRectangleRounded(fillRect, 0.3f, 10, fillColor);

    Color borderColor = isHovered() || _dragging ? SKYBLUE : Fade(DARKGRAY, 0.5f);
    DrawRectangleRoundedLines(barRect, 0.3f, 10, borderColor);

    Vector2 handlePos = getHandlePosition();
    Color handleColor = isHovered() || _dragging ? SKYBLUE : RAYWHITE;
    DrawCircleV(handlePos, _handleRadius, handleColor);
    DrawCircleLines(handlePos.x, handlePos.y, _handleRadius, WHITE);
    DrawCircleV({handlePos.x + 2, handlePos.y + 3}, _handleRadius, Fade(BLACK, 0.10f));

    char valueText[32];
    int displayValue = static_cast<int>((_value - _min) / (_max - _min) * 100);
    snprintf(valueText, sizeof(valueText), "%d", displayValue);

    Font font = FontManager::getInstance().getFont("medium");
    Vector2 textSize = MeasureTextEx(font, valueText, 20, 1);
    DrawTextEx(font, valueText, {_position.x + _originalWidth + 20, _position.y + (_size.y - textSize.y) / 2}, 20, 1, SKYBLUE);
}

void Slider::setCallback(const std::function<void(float)> &callback)
{
    _callback = callback;
}

void Slider::setValue(float value)
{
    _value = std::max(_min, std::min(_max, value));
    if (_callback) {
        _callback(_value);
    }
}

float Slider::getValue() const
{
    return _value;
}

void Slider::setLabel(const std::string &label)
{
    _label = label;
}

Rectangle Slider::getSliderBar() const
{
    return {
        _position.x,
        _position.y + (_size.y - _handleSize / 2) / 2,
        _originalWidth,
        _handleSize / 2
    };
}

Vector2 Slider::getHandlePosition() const
{
    float t = (_value - _min) / (_max - _min);
    float x = _position.x + t * _originalWidth;
    float y = _position.y + _size.y / 2;
    return { x, y };
}
