/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ChatSystem
*/

#include "ChatSystem.hpp"
#include "FontManager.hpp"
#include <algorithm>

ChatSystem& ChatSystem::getInstance() {
    static ChatSystem instance;
    return instance;
}

void ChatSystem::initialize() {
    _messages.clear();
    _totalTime = 0.0f;
    addMessage("System", "Welcome to Zappy!", GREEN);
    addMessage("System", "Press C to toggle chat visibility", LIGHTGRAY);
}

void ChatSystem::update(float dt) {
    _totalTime += dt;
    if (IsKeyPressed(KEY_C)) {
        toggleVisibility();
    }
}

void ChatSystem::draw() {
    if (!_visible || _messages.empty()) return;
    drawChatBox();
}

void ChatSystem::addMessage(const std::string& sender, const std::string& content, Color color) {
    ChatMessage msg;
    msg.sender = sender;
    msg.content = content;
    msg.color = color;
    msg.timestamp = _totalTime;
    _messages.push_back(msg);
    if (_messages.size() > static_cast<size_t>(MAX_MESSAGES)) {
        _messages.erase(_messages.begin());
    }
}

void ChatSystem::toggleVisibility() {
    _visible = !_visible;
    addMessage("System", _visible ? "Chat visible" : "Chat hidden", GRAY);
}

void ChatSystem::drawChatBox() {
    int chatX = GetScreenWidth() - 420;
    int chatY = GetScreenHeight() - 220;
    Font font = FontManager::getInstance().getFont("medium");
    DrawRectangle(chatX, chatY, 400, 200, Fade(BLACK, 0.7f));
    DrawRectangleLines(chatX, chatY, 400, 200, DARKGRAY);
    DrawTextEx(font, "CHAT (C to toggle)", {(float)(chatX + 10), (float)(chatY + 10)}, 16, 1, WHITE);
    for (size_t i = 0; i < _messages.size(); ++i) {
        const ChatMessage& msg = _messages[i];
        float age = _totalTime - msg.timestamp;
        float alpha = (age < MESSAGE_FADE_TIME) ? 1.0f : std::max(0.0f, 1.0f - (age - MESSAGE_FADE_TIME) / 2.0f);
        if (alpha > 0.0f) {
            Color msgColor = Fade(msg.color, alpha);
            std::string fullMessage = msg.sender + ": " + msg.content;
            DrawTextEx(font, fullMessage.c_str(), {(float)(chatX + 10), (float)(chatY + 40 + i * 25)}, 13, 1, msgColor);
        }
    }
}
