/*
** EPITECH PROJECT, 2025
** src/gui/ui/KeyBindButton.hpp
** File description:
** KeyBindButton
*/

#ifndef KEYBINDBUTTON_HPP_
    #define KEYBINDBUTTON_HPP_

#include "AComponent.hpp"

#include <string>
#include <functional>
#include <vector>
#include <memory>

class KeyBindButton : public AComponent {
public:
    KeyBindButton(const Vector2 &position, const Vector2 &size, const std::string &action, int keyCode);
    ~KeyBindButton();

    void update(float dt) override;
    void draw() const override;

    void setCallback(const std::function<void(int)> &callback);
    void setKeyCode(int keyCode);
    int getKeyCode() const;
    std::string getAction() const;

    static void clearListeningState();

private:
    std::string _action;
    int _keyCode;
    std::function<void(int)> _callback;
    bool _listening = false;

    std::string getKeyName(int keyCode) const;

    static std::vector<KeyBindButton*> _allKeyBinds;
    static void registerInstance(KeyBindButton* instance);
    static void unregisterInstance(KeyBindButton* instance);
    static KeyBindButton* findByKeyCode(int keyCode, KeyBindButton* except = nullptr);
};

#endif /* !KEYBINDBUTTON_HPP_ */
