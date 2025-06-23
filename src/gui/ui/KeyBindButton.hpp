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
#include <mutex>

class KeyBindButton : public AComponent {
public:
    KeyBindButton(const Vector2 &position, const Vector2 &size, const std::string &action, int keyCode);
    ~KeyBindButton();

    // Rule of 5
    KeyBindButton(const KeyBindButton&) = delete;
    KeyBindButton& operator=(const KeyBindButton&) = delete;
    KeyBindButton(KeyBindButton&&) = delete;
    KeyBindButton& operator=(KeyBindButton&&) = delete;

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

    // Thread-safe static member management
    struct InstanceManager {
        std::vector<KeyBindButton*> instances;
        std::mutex mutex;
        
        void registerInstance(KeyBindButton* instance);
        void unregisterInstance(KeyBindButton* instance);
        KeyBindButton* findByKeyCode(int keyCode, KeyBindButton* except = nullptr);
        void clearAllListening();
    };
    
    static InstanceManager& getInstanceManager();
};

#endif /* !KEYBINDBUTTON_HPP_ */
