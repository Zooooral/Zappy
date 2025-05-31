/*
** EPITECH PROJECT, 2025
** src/gui/ui/Slider.hpp
** File description:
** Slider
*/

#ifndef SLIDER_HPP_
    #define SLIDER_HPP_

#include "AComponent.hpp"

#include <string>
#include <functional>

class Slider : public AComponent {
public:
    Slider(const Vector2 &position, const Vector2 &size, float min, float max, float value);
    ~Slider() = default;

    void update(float dt) override;
    void draw() const override;

    void setCallback(const std::function<void(float)> &callback);
    void setValue(float value);
    float getValue() const;
    void setLabel(const std::string &label);

private:
    std::string _label;
    float _min;
    float _max;
    float _value;
    float _originalWidth;
    std::function<void(float)> _callback;
    bool _dragging = false;

    float _handleSize = 20.0f;
    float _handleRadius = 10.0f;

    Rectangle getSliderBar() const;
    Vector2 getHandlePosition() const;
};

#endif /* !SLIDER_HPP_ */
