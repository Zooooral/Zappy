/*
** EPITECH PROJECT, 2025
** src/gui/screens/ConnectingScreen.hpp
** File description:
** Beautiful connecting screen for server connection
*/

#ifndef CONNECTINGSCREEN_HPP_
    #define CONNECTINGSCREEN_HPP_

#include "../core/GameState.hpp"
#include "../ui/Button.hpp"
#include "raylib.h"

#include <memory>
#include <string>
#include <vector>

class ConnectingScreen : public GameState {
public:
    ConnectingScreen();
    ~ConnectingScreen();

    void update(float dt) override;
    void draw() override;
    bool isFinished() const override;
    bool hasShaderBackground() const override;

    void onEnter() override;
    void onExit() override;

    void setConnectionInfo(const std::string& host, int port);
    void setConnectionStatus(const std::string& status);
    void setConnectionError(const std::string& error);
    void setConnected(bool connected);

private:
    struct Particle {
        Vector2 position;
        Vector2 velocity;
        float life;
        float maxLife;
        float size;
        Color color;
    };

    struct ConnectionDot {
        Vector2 position;
        float animation;
        float delay;
        bool active;
    };

    Shader _shader;
    int _timeLoc;
    int _resolutionLoc;
    int _alphaLoc;

    std::unique_ptr<Button> _cancelButton;
    std::unique_ptr<Button> _retryButton;

    std::string _host;
    int _port;
    std::string _connectionStatus;
    std::string _connectionError;
    bool _isConnected;
    bool _finished;
    bool _showError;
    bool _transitionStarted;

    float _animationTimer;
    float _pulseTimer;
    float _particleTimer;

    std::vector<Particle> _particles;
    std::vector<ConnectionDot> _connectionDots;

    void initializeComponents();
    void updateParticles(float dt);
    void updateConnectionAnimation(float dt);
    void drawBackground();
    void drawConnectionVisual();
    void drawStatusText();
    void drawParticles();
    void createParticles();
    void setupConnectionDots();
    Color getStatusColor() const;
};

#endif /* !CONNECTINGSCREEN_HPP_ */
