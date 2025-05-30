/*
** EPITECH PROJECT, 2025
** src/gui/ui/KeyBindButton.cpp
** File description:
** KeyBindButton
*/

#include "KeyBindButton.hpp"
#include "../core/FontManager.hpp"

#include <algorithm>

std::vector<KeyBindButton*> KeyBindButton::_allKeyBinds;

KeyBindButton::KeyBindButton(const Vector2 &position, const Vector2 &size, const std::string &action, int keyCode) : AComponent(position, size), _action(action), _keyCode(keyCode)
{
    registerInstance(this);
}

KeyBindButton::~KeyBindButton()
{
    unregisterInstance(this);
}

void KeyBindButton::update(float dt)
{
    (void)dt;
    float buttonWidth = 150;
    float buttonX = _position.x + _size.x - buttonWidth;
    float buttonY = _position.y + (_size.y - 40) / 2;
    Rectangle buttonRect = { buttonX, buttonY, buttonWidth, 40 };

    if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        clearListeningState();
        _listening = true;
    }
    if (_listening) {
        int key = GetKeyPressed();
        if (key > 0) {
            KeyBindButton* existingBind = findByKeyCode(key, this);
            if (existingBind) {
                existingBind->setKeyCode(0);
                if (existingBind->_callback) {
                    existingBind->_callback(0);
                }
            }
            _keyCode = key;
            _listening = false;
            if (_callback) {
                _callback(_keyCode);
            }
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            _listening = false;
        }
    }
}

void KeyBindButton::draw() const
{
    Color actionColor = WHITE;
    Color keyColor = _listening ? BLUE : WHITE;
    Color bgColor = Fade(BLACK, 0.5f);

    Font font = FontManager::getInstance().getFont("medium");
    DrawTextEx(font, _action.c_str(), {_position.x + 10, _position.y + (_size.y - 24) / 2}, 24, 1, actionColor);

    std::string keyName = _listening ? "Press a key..." : getKeyName(_keyCode);
    Vector2 keyNameSize = MeasureTextEx(font, keyName.c_str(), 24, 1);

    float buttonWidth = 150;
    float buttonX = _position.x + _size.x - buttonWidth;
    float buttonY = _position.y + (_size.y - 40) / 2;
    Rectangle buttonRect = { buttonX, buttonY, buttonWidth, 40 };

    bool isBoxHovered = CheckCollisionPointRec(GetMousePosition(), buttonRect);
    Color buttonBgColor = _listening ? Fade(BLUE, 0.2f) : bgColor;
    Color buttonBorderColor = _listening ? BLUE : (isBoxHovered ? WHITE : DARKGRAY);

    DrawRectangleRounded(buttonRect, 0.3f, 10, buttonBgColor);
    DrawRectangleRoundedLines(buttonRect, 0.3f, 10, buttonBorderColor);

    float textX = buttonX + (buttonWidth - keyNameSize.x) / 2;
    float textY = buttonY + (40 - keyNameSize.y) / 2;

    DrawTextEx(font, keyName.c_str(), {textX, textY}, 24, 1, keyColor);
}

void KeyBindButton::setCallback(const std::function<void(int)> &callback)
{
    _callback = callback;
}

void KeyBindButton::setKeyCode(int keyCode)
{
    _keyCode = keyCode;
}

int KeyBindButton::getKeyCode() const
{
    return _keyCode;
}

std::string KeyBindButton::getAction() const
{
    return _action;
}

std::string KeyBindButton::getKeyName(int keyCode) const
{
    switch (keyCode) {
        case 0: return "NONE";
        case KEY_A: return "A";
        case KEY_B: return "B";
        case KEY_C: return "C";
        case KEY_D: return "D";
        case KEY_E: return "E";
        case KEY_F: return "F";
        case KEY_G: return "G";
        case KEY_H: return "H";
        case KEY_I: return "I";
        case KEY_J: return "J";
        case KEY_K: return "K";
        case KEY_L: return "L";
        case KEY_M: return "M";
        case KEY_N: return "N";
        case KEY_O: return "O";
        case KEY_P: return "P";
        case KEY_Q: return "Q";
        case KEY_R: return "R";
        case KEY_S: return "S";
        case KEY_T: return "T";
        case KEY_U: return "U";
        case KEY_V: return "V";
        case KEY_W: return "W";
        case KEY_X: return "X";
        case KEY_Y: return "Y";
        case KEY_Z: return "Z";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_SPACE: return "SPACE";
        case KEY_ESCAPE: return "ESC";
        case KEY_ENTER: return "ENTER";
        default: return "UNKNOWN";
    }
}

void KeyBindButton::registerInstance(KeyBindButton* instance)
{
    _allKeyBinds.push_back(instance);
}

void KeyBindButton::unregisterInstance(KeyBindButton* instance)
{
    auto it = std::find(_allKeyBinds.begin(), _allKeyBinds.end(), instance);
    if (it != _allKeyBinds.end()) {
        _allKeyBinds.erase(it);
    }
}

void KeyBindButton::clearListeningState()
{
    for (KeyBindButton* button : _allKeyBinds) {
        button->_listening = false;
    }
}

KeyBindButton* KeyBindButton::findByKeyCode(int keyCode, KeyBindButton* except)
{
    for (KeyBindButton* button : _allKeyBinds) {
        if (button != except && button->_keyCode == keyCode) {
            return button;
        }
    }
    return nullptr;
}
// Updated: 2025-05-31 00:53:22
