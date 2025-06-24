/*
** EPITECH PROJECT, 2025
** src/gui/screens/ConnectingScreen.cpp
** File description:
** Beautiful connecting screen implementation
*/

#include <iostream>
#include <cmath>
#include <algorithm>

#include "ConnectingScreen.hpp"
#include "../core/FontManager.hpp"
#include "../core/SoundManager.hpp"
#include "../core/GameStateManager.hpp"
#include "../network/NetworkManager.hpp"
#include "../core/ConfigManager.hpp"

ConnectingScreen::ConnectingScreen()
{
    _shader = LoadShader(NULL, "assets/shaders/main_menu.glsl");
    _timeLoc = GetShaderLocation(_shader, "iTime");
    _resolutionLoc = GetShaderLocation(_shader, "iResolution");
    _alphaLoc = GetShaderLocation(_shader, "iAlpha");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(_shader, _resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

    _host = "localhost";
    _port = 4242;
    _connectionStatus = "Initializing...";
    _connectionError = "";
    _isConnected = false;
    _finished = false;
    _showError = false;
    _transitionStarted = false;
    _animationTimer = 0.0f;
    _pulseTimer = 0.0f;
    _particleTimer = 0.0f;

    initializeComponents();
    setupConnectionDots();
}

ConnectingScreen::~ConnectingScreen()
{
    if (IsWindowReady()) {
        UnloadShader(_shader);
    }
}

void ConnectingScreen::onEnter()
{
    setTransitionAlpha(1.0f);
    _finished = false;
    _showError = false;
    _transitionStarted = false;
    _animationTimer = 0.0f;
    _pulseTimer = 0.0f;
    _particleTimer = 0.0f;
    _particles.clear();

    ConfigManager& config = ConfigManager::getInstance();
    _host = config.getHost();
    _port = config.getPort();

    _connectionStatus = "Connecting to " + _host + ":" + std::to_string(_port);

    NetworkManager& network = NetworkManager::getInstance();
    network.setMessageCallback([](const std::string&) {});

    if (!network.connectToServer(_host, _port)) {
        setConnectionError("Failed to connect to server");
    }
}

void ConnectingScreen::onExit()
{
}

void ConnectingScreen::initializeComponents()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    _cancelButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 400), (float)(screenH - 100)},
        Vector2{180, 50},
        "Cancel"
    );
    _cancelButton->setCallback([this]() {
        NetworkManager::getInstance().disconnect();
        GameStateManager::getInstance().changeState("main_menu", 
            GameStateManager::Transition::FADE);
    });

    _retryButton = std::make_unique<Button>(
        Vector2{(float)(screenW - 200), (float)(screenH - 100)},
        Vector2{180, 50},
        "Retry"
    );
    _retryButton->setCallback([this]() {
        _showError = false;
        _connectionError = "";
        _connectionStatus = "Retrying connection...";
        _transitionStarted = false;
        NetworkManager::getInstance().disconnect();
        if (!NetworkManager::getInstance().connectToServer(_host, _port)) {
            setConnectionError("Failed to connect to server");
        }
    });
}

void ConnectingScreen::setupConnectionDots()
{
    _connectionDots.clear();
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    for (int i = 0; i < 8; i++) {
        ConnectionDot dot;
        dot.position = Vector2{
            (float)(screenW * 0.3f + i * 60),
            (float)(screenH * 0.6f)
        };
        dot.animation = 0.0f;
        dot.delay = i * 0.15f;
        dot.active = false;
        _connectionDots.push_back(dot);
    }
}

void ConnectingScreen::setConnectionInfo(const std::string& host, int port)
{
    _host = host;
    _port = port;
}

void ConnectingScreen::setConnectionStatus(const std::string& status)
{
    _connectionStatus = status;
    _showError = false;
}

void ConnectingScreen::setConnectionError(const std::string& error)
{
    _connectionError = error;
    _showError = true;
}

void ConnectingScreen::setConnected(bool connected)
{
    _isConnected = connected;
    if (connected) {
        _connectionStatus = "Connected successfully!";
        _finished = true;
    }
}

void ConnectingScreen::update(float dt)
{
    _animationTimer += dt;
    _pulseTimer += dt;
    _particleTimer += dt;

    float time = (float)GetTime();
    SetShaderValue(_shader, _timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shader, _alphaLoc, &_transitionAlpha, SHADER_UNIFORM_FLOAT);

    NetworkManager& network = NetworkManager::getInstance();
    network.update();

    ConnectionState state = network.getConnectionState();

    switch (state) {
        case ConnectionState::CONNECTING:
            setConnectionStatus("Connecting to server...");
            break;
        case ConnectionState::CONNECTED:
            setConnectionStatus("Authenticating...");
            break;
        case ConnectionState::AUTHENTICATED:
            if (!_finished) {
                std::cout << "[GUI] Authentication successful! Starting transition..." << std::endl;
                setConnectionStatus("Connected successfully!");
                _finished = true;
                _isConnected = true;
            }
            break;
        case ConnectionState::ERROR:
            setConnectionError("Connection failed - Server unreachable");
            break;
        case ConnectionState::DISCONNECTED:
            if (!_showError) {
                setConnectionStatus("Disconnected");
            }
            break;
    }

    updateParticles(dt);
    updateConnectionAnimation(dt);

    if (_particleTimer > 0.1f) {
        createParticles();
        _particleTimer = 0.0f;
    }

    _cancelButton->update(dt);
    if (_showError) {
        _retryButton->update(dt);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        NetworkManager::getInstance().disconnect();
        GameStateManager::getInstance().changeState("main_menu", 
            GameStateManager::Transition::FADE);
    }

    if (_finished && !_transitionStarted && _animationTimer > 0.5f) {
        std::cout << "[GUI] Initiating transition to game screen..." << std::endl;
        _transitionStarted = true;
        GameStateManager::getInstance().changeState("game",
            GameStateManager::Transition::FADE);
    }
}

void ConnectingScreen::updateParticles(float dt)
{
    for (auto& particle : _particles) {
        particle.life -= dt;
        particle.position.x += particle.velocity.x * dt;
        particle.position.y += particle.velocity.y * dt;

        float lifeRatio = particle.life / particle.maxLife;
        particle.color.a = (unsigned char)(255 * lifeRatio);
        particle.size = 2.0f + sinf(particle.life * 5.0f) * 1.0f;
    }

    _particles.erase(
        std::remove_if(_particles.begin(), _particles.end(),
                      [](const Particle& p) { return p.life <= 0.0f; }),
        _particles.end()
    );
}

void ConnectingScreen::updateConnectionAnimation(float dt)
{
    for (auto& dot : _connectionDots) {
        if (_animationTimer > dot.delay) {
            dot.active = true;
            dot.animation += dt * 3.0f;
            if (dot.animation > 2.0f * PI) {
                dot.animation = 0.0f;
            }
        }
    }
}

void ConnectingScreen::createParticles()
{
    if (_particles.size() >= 50) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    for (int i = 0; i < 3; i++) {
        Particle particle;
        particle.position = Vector2{
            (float)(rand() % screenW),
            (float)(screenH + 20)
        };
        particle.velocity = Vector2{
            (float)((rand() % 100 - 50) / 100.0f),
            (float)(-(rand() % 50 + 20) / 10.0f)
        };
        particle.life = particle.maxLife = (float)(rand() % 300 + 200) / 100.0f;
        particle.size = (float)(rand() % 4 + 2);

        Color particleColor = getStatusColor();
        particleColor.a = (unsigned char)(rand() % 128 + 127);
        particle.color = particleColor;

        _particles.push_back(particle);
    }
}

Color ConnectingScreen::getStatusColor() const
{
    if (_showError) return RED;
    if (_isConnected) return GREEN;

    float pulse = (sinf(_pulseTimer * 4.0f) + 1.0f) * 0.5f;
    return ColorLerp(SKYBLUE, BLUE, pulse);
}

void ConnectingScreen::draw()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    drawBackground();
    drawConnectionVisual();
    drawParticles();
    drawStatusText();

    _cancelButton->draw();
    if (_showError) {
        _retryButton->draw();
    }

    if (!_showError && !_finished) {
        float progress = fmodf(_animationTimer * 0.5f, 1.0f);
        int barWidth = 400;
        int barHeight = 4;
        int barX = (screenW - barWidth) / 2;
        int barY = screenH * 0.75f;

        DrawRectangle(barX, barY, barWidth, barHeight, Fade(WHITE, 0.3f));
        DrawRectangle(barX, barY, (int)(barWidth * progress), barHeight, 
                     getStatusColor());
    }
}

void ConnectingScreen::drawBackground()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BLACK);

    BeginShaderMode(_shader);
    DrawRectangle(0, 0, screenW, screenH, Fade(WHITE, _transitionAlpha * 0.8f));
    EndShaderMode();
}

void ConnectingScreen::drawConnectionVisual()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    for (const auto& dot : _connectionDots) {
        if (!dot.active) continue;

        float scale = 1.0f + sinf(dot.animation) * 0.3f;
        float alpha = 0.7f + sinf(dot.animation * 2.0f) * 0.3f;

        Color dotColor = getStatusColor();
        dotColor.a = (unsigned char)(255 * alpha * _transitionAlpha);

        DrawCircleV(dot.position, 8.0f * scale, dotColor);
        DrawRing(dot.position, 8.0f * scale, 12.0f * scale, 0, 360, 32, 
                Fade(dotColor, 0.3f));
    }

    for (size_t i = 0; i < _connectionDots.size() - 1; i++) {
        if (_connectionDots[i].active && _connectionDots[i + 1].active) {
            float alpha = sinf(_animationTimer * 2.0f + i) * 0.5f + 0.5f;
            Color lineColor = getStatusColor();
            lineColor.a = (unsigned char)(255 * alpha * _transitionAlpha * 0.6f);

            DrawLineEx(_connectionDots[i].position, _connectionDots[i + 1].position,
                      2.0f, lineColor);
        }
    }

    Vector2 center = Vector2{(float)(screenW * 0.5f), (float)(screenH * 0.45f)};
    float hubPulse = 1.0f + sinf(_pulseTimer * 3.0f) * 0.2f;
    Color hubColor = getStatusColor();
    hubColor.a = (unsigned char)(255 * _transitionAlpha);

    DrawCircleV(center, 30.0f * hubPulse, Fade(hubColor, 0.3f));
    DrawCircleV(center, 20.0f * hubPulse, hubColor);
    DrawRing(center, 25.0f * hubPulse, 35.0f * hubPulse, 0, 360, 32, 
            Fade(hubColor, 0.5f));
}

void ConnectingScreen::drawStatusText()
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Font fontBold = FontManager::getInstance().getFont("bold");
    Font fontMedium = FontManager::getInstance().getFont("medium");

    const char* title = "CONNECTING TO ZAPPY";
    int titleSize = 48;
    Vector2 titleTextSize = MeasureTextEx(fontBold, title, titleSize, 1);
    DrawTextEx(fontBold, title,
               {(float)(screenW - titleTextSize.x) / 2, screenH * 0.25f},
               titleSize, 1, Fade(WHITE, _transitionAlpha));

    Color statusColor = _showError ? RED : getStatusColor();
    statusColor.a = (unsigned char)(255 * _transitionAlpha);

    Vector2 statusSize = MeasureTextEx(fontMedium, _connectionStatus.c_str(), 24, 1);
    DrawTextEx(fontMedium, _connectionStatus.c_str(),
               {(float)(screenW - statusSize.x) / 2, screenH * 0.8f},
               24, 1, statusColor);

    if (_showError && !_connectionError.empty()) {
        Vector2 errorSize = MeasureTextEx(fontMedium, _connectionError.c_str(), 20, 1);
        DrawTextEx(fontMedium, _connectionError.c_str(),
                   {(float)(screenW - errorSize.x) / 2, screenH * 0.85f},
                   20, 1, Fade(RED, _transitionAlpha));
    }

    const char* instruction = _showError ?
        "Press ESCAPE to return to menu or click Retry" :
        "Press ESCAPE to cancel connection";
    Vector2 instrSize = MeasureTextEx(fontMedium, instruction, 16, 1);
    DrawTextEx(fontMedium, instruction,
               {(float)(screenW - instrSize.x) / 2, screenH * 0.92f},
               16, 1, Fade(LIGHTGRAY, _transitionAlpha));
}

void ConnectingScreen::drawParticles()
{
    for (const auto& particle : _particles) {
        DrawCircleV(particle.position, particle.size, particle.color);
    }
}

bool ConnectingScreen::isFinished() const
{
    return _finished;
}

bool ConnectingScreen::hasShaderBackground() const
{
    return true;
}
