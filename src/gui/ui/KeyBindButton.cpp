/*
** EPITECH PROJECT, 2025
** src/gui/ui/KeyBindButton.cpp
** File description:
** KeyBindButton implementation with safe static management
*/

#include "KeyBindButton.hpp"
#include "../core/FontManager.hpp"
#include "../core/Constants.hpp"
#include <algorithm>

KeyBindButton::KeyBindButton(const Vector2 &position, const Vector2 &size, const std::string &action, int keyCode)
    : AComponent(position, size), _action(action), _keyCode(keyCode) {
    try {
        getInstanceManager().registerInstance(this);
    } catch (...) {
    }
}

KeyBindButton::~KeyBindButton() {
    static bool isShuttingDown = false;
    if (!isShuttingDown) {
        try {
            getInstanceManager().unregisterInstance(this);
        } catch (...) {
            isShuttingDown = true;
        }
    }
}

void KeyBindButton::update(float dt) {
    (void)dt;
    
    if (isClicked()) {
        clearListeningState();
        _listening = true;
    }

    if (_listening) {
        for (int key = 0; key < 400; key++) {
            if (IsKeyPressed(key)) {
                auto& manager = getInstanceManager();
                KeyBindButton* conflicting = manager.findByKeyCode(key, this);
                
                if (conflicting != nullptr) {
                    conflicting->setKeyCode(0);
                }
                
                setKeyCode(key);
                _listening = false;
                
                if (_callback) {
                    _callback(key);
                }
                break;
            }
        }
    }
}

void KeyBindButton::draw() const {
    if (!_visible || !_enabled) return;

    Font font = FontManager::getInstance().getFont("medium");
    
    Vector2 labelSize = MeasureTextEx(font, _action.c_str(), zappy::constants::FONT_SIZE_MEDIUM, 1);
    float labelX = _position.x;
    float labelY = _position.y + (_size.y - labelSize.y) / 2;
    DrawTextEx(font, _action.c_str(), {labelX, labelY}, zappy::constants::FONT_SIZE_MEDIUM, 1, RAYWHITE);

    Color bgColor = Fade(BLACK, 0.5f);
    Color keyColor = _listening ? YELLOW : RAYWHITE;
    
    std::string keyName = _listening ? "Press a key..." : getKeyName(_keyCode);
    Vector2 keyNameSize = MeasureTextEx(font, keyName.c_str(), zappy::constants::FONT_SIZE_MEDIUM, 1);

    constexpr float buttonWidth = 150.0f;
    constexpr float buttonHeight = 40.0f;
    float buttonX = _position.x + _size.x - buttonWidth;
    float buttonY = _position.y + (_size.y - buttonHeight) / 2;
    Rectangle buttonRect = { buttonX, buttonY, buttonWidth, buttonHeight };

    bool isBoxHovered = CheckCollisionPointRec(GetMousePosition(), buttonRect);
    Color buttonBgColor = _listening ? Fade(BLUE, 0.2f) : bgColor;
    Color buttonBorderColor = _listening ? BLUE : (isBoxHovered ? WHITE : DARKGRAY);

    DrawRectangleRounded(buttonRect, 0.3f, 10, buttonBgColor);
    DrawRectangleRoundedLines(buttonRect, 0.3f, 10, buttonBorderColor);

    float textX = buttonX + (buttonWidth - keyNameSize.x) / 2;
    float textY = buttonY + (buttonHeight - keyNameSize.y) / 2;

    DrawTextEx(font, keyName.c_str(), {textX, textY}, zappy::constants::FONT_SIZE_MEDIUM, 1, keyColor);
}

void KeyBindButton::setCallback(const std::function<void(int)> &callback) {
    _callback = callback;
}

void KeyBindButton::setKeyCode(int keyCode) {
    _keyCode = keyCode;
}

int KeyBindButton::getKeyCode() const {
    return _keyCode;
}

std::string KeyBindButton::getAction() const {
    return _action;
}

std::string KeyBindButton::getKeyName(int keyCode) const {
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

KeyBindButton::InstanceManager& KeyBindButton::getInstanceManager() {
    static InstanceManager manager;
    return manager;
}

void KeyBindButton::InstanceManager::registerInstance(KeyBindButton* instance) {
    if (!instance) return;
    
    try {
        std::lock_guard<std::mutex> lock(mutex);
        instances.push_back(instance);
    } catch (...) {
    }
}

void KeyBindButton::InstanceManager::unregisterInstance(KeyBindButton* instance) {
    if (!instance) return;
    
    try {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = std::find(instances.begin(), instances.end(), instance);
        if (it != instances.end()) {
            instances.erase(it);
        }
    } catch (...) {
    }
}

void KeyBindButton::InstanceManager::clearAllListening() {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        for (KeyBindButton* button : instances) {
            if (button) {
                button->_listening = false;
            }
        }
    } catch (...) {
    }
}

KeyBindButton* KeyBindButton::InstanceManager::findByKeyCode(int keyCode, KeyBindButton* except) {
    try {
        std::lock_guard<std::mutex> lock(mutex);
        for (KeyBindButton* button : instances) {
            if (button && button != except && button->_keyCode == keyCode) {
                return button;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

void KeyBindButton::clearListeningState() {
    try {
        getInstanceManager().clearAllListening();
    } catch (...) {
    }
}