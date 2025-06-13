/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ChatSystem
*/

#ifndef CHATSYSTEM_HPP_
    #define CHATSYSTEM_HPP_

    #include <vector>
    #include <string>

    #include "raylib.h"

struct ChatMessage {
    std::string sender;
    std::string content;
    Color color;
    float timestamp;
};

class ChatSystem {
public:
    static ChatSystem& getInstance();
    void initialize();
    void update(float dt);
    void draw();
    void addMessage(const std::string& sender, const std::string& content, Color color = WHITE);
    void toggleVisibility();
    bool isVisible() const { return _visible; }
private:
    ChatSystem() = default;
    ~ChatSystem() = default;
    ChatSystem(const ChatSystem&) = delete;
    ChatSystem& operator=(const ChatSystem&) = delete;
    std::vector<ChatMessage> _messages;
    bool _visible = true;
    float _totalTime = 0.0f;
    const int MAX_MESSAGES = 6;
    const float MESSAGE_FADE_TIME = 10.0f;
    void drawChatBox();
};

#endif
