/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** DebugSystem
*/

#ifndef DEBUGSYSTEM_HPP_
    #define DEBUGSYSTEM_HPP_

    #include <vector>
    #include <string>
    #include <memory>

    #include "raylib.h"

class GameWorld;

class DebugSystem {
public:
    static DebugSystem& getInstance();
    void initialize();
    void update(float dt);
    void draw();
    void toggleDebugMenu();
    void toggleDebugSpheres();
    void toggleWireframe();
    void toggleTileInfo();
    void toggleObstacles();
    bool isDebugMenuVisible() const { return _showDebugMenu; }
    bool showDebugSpheres() const { return _showDebugSpheres; }
    bool showWireframe() const { return _showWireframe; }
    bool showTileInfo() const { return _showTileInfo; }
    bool showObstacles() const { return _showObstacles; }
    void addLogMessage(const std::string& message);
    void setSelectedTile(int x, int y);
    void clearSelectedTile();
private:
    DebugSystem() = default;
    ~DebugSystem() = default;
    DebugSystem(const DebugSystem&) = delete;
    DebugSystem& operator=(const DebugSystem&) = delete;
    bool _showDebugMenu = false;
    bool _showDebugSpheres = false;
    bool _showWireframe = false;
    bool _showTileInfo = true;
    bool _showCoordinates = true;
    bool _showObstacles = true;
    int _selectedTileX = -1;
    int _selectedTileY = -1;
    std::vector<std::string> _logMessages;
    float _fpsTimer = 0.0f;
    int _frameCount = 0;
    float _averageFPS = 0.0f;
    void drawDebugMenu();
    void drawTileInfo();
    void drawLogMessages();
};

#endif
