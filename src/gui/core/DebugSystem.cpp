/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** DebugSystem
*/

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "DebugSystem.hpp"
#include "GameWorld.hpp"
#include "CameraController.hpp"
#include "TileInteraction.hpp"
#include "ChatSystem.hpp"
#include "FontManager.hpp"

DebugSystem& DebugSystem::getInstance() {
    static DebugSystem instance;
    return instance;
}

void DebugSystem::initialize() {
    ChatSystem::getInstance().initialize();
    ChatSystem::getInstance().addMessage("Debug", "Debug system initialized", YELLOW);
}

void DebugSystem::update(float dt) {
    if (IsKeyPressed(KEY_F2)) toggleDebugMenu();
    if (IsKeyPressed(KEY_F3)) toggleDebugSpheres();
    if (IsKeyPressed(KEY_F4)) toggleWireframe();
    if (IsKeyPressed(KEY_F5)) toggleTileInfo();
    if (IsKeyPressed(KEY_F6)) toggleObstacles();
    ChatSystem::getInstance().update(dt);
}

void DebugSystem::draw() {
    if (_showDebugMenu) drawDebugMenu();
    if (_showTileInfo) drawTileInfo();
    ChatSystem::getInstance().draw();
}

void DebugSystem::toggleDebugMenu() {
    _showDebugMenu = !_showDebugMenu;
    ChatSystem::getInstance().addMessage("Debug", 
        "Debug menu: " + std::string(_showDebugMenu ? "ON" : "OFF"), YELLOW);
}

void DebugSystem::toggleDebugSpheres() {
    _showDebugSpheres = !_showDebugSpheres;
    ChatSystem::getInstance().addMessage("Debug", 
        "Debug spheres: " + std::string(_showDebugSpheres ? "ON" : "OFF"), YELLOW);
}

void DebugSystem::toggleWireframe() {
    _showWireframe = !_showWireframe;
    ChatSystem::getInstance().addMessage("Debug", 
        "Wireframe: " + std::string(_showWireframe ? "ON" : "OFF"), YELLOW);
}

void DebugSystem::toggleTileInfo() {
    _showTileInfo = !_showTileInfo;
    ChatSystem::getInstance().addMessage("Debug", 
        "Tile info: " + std::string(_showTileInfo ? "ON" : "OFF"), YELLOW);
}

void DebugSystem::toggleObstacles() {
    _showObstacles = !_showObstacles;
    ChatSystem::getInstance().addMessage("Debug", 
        "Obstacles: " + std::string(_showObstacles ? "ON" : "OFF"), YELLOW);
}

void DebugSystem::addLogMessage(const std::string& message) {
    ChatSystem::getInstance().addMessage("System", message, LIGHTGRAY);
}

void DebugSystem::setSelectedTile(int x, int y) {
    _selectedTileX = x;
    _selectedTileY = y;
}

void DebugSystem::clearSelectedTile() {
    _selectedTileX = -1;
    _selectedTileY = -1;
}

void DebugSystem::drawDebugMenu() {
    int startY = 130;
    int lineHeight = 30;
    int currentY = startY;
    Font font = FontManager::getInstance().getFont("medium");
    DrawRectangle(10, startY - 5, 340, 510, Fade(BLACK, 0.8f));
    DrawRectangleLines(10, startY - 5, 340, 510, LIGHTGRAY);
    DrawTextEx(font, "DEBUG MENU", {20, (float)currentY}, 24, 1, YELLOW);
    currentY += lineHeight * 1.5f;
    std::string controls[] = {
        "F3 - Toggle Debug Spheres: " + std::string(_showDebugSpheres ? "ON" : "OFF"),
        "F4 - Toggle Wireframe: " + std::string(_showWireframe ? "ON" : "OFF"),
        "F5 - Toggle Tile Info: " + std::string(_showTileInfo ? "ON" : "OFF"),
        "F6 - Toggle Obstacles: " + std::string(_showObstacles ? "ON" : "OFF"),
        "",
        "C - Toggle Chat Window",
        "P - Toggle View Mode",
        "Z/Q/S/D - Move Camera",
        "Middle Click + Drag - Rotate Camera (3D)",
        "Right Click + Drag - Pan Camera",
        "Mouse Wheel - Zoom in/out",
        "Left Click - Select tile",
        "SHIFT - Move faster"
    };
    for (const auto& control : controls) {
        if (!control.empty()) {
            DrawTextEx(font, control.c_str(), {20, (float)currentY}, 16, 1, WHITE);
        }
        currentY += lineHeight;
    }
    currentY += lineHeight * 0.5f;
    DrawTextEx(font, "Camera Info:", {20, (float)currentY}, 18, 1, SKYBLUE);
    currentY += lineHeight;
    CameraController& cam = CameraController::getInstance();
    if (cam.is3DMode()) {
        Camera camera = cam.getCamera3D();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1);
        oss << "Position: (" << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << ")";
        DrawTextEx(font, oss.str().c_str(), {20, (float)currentY}, 16, 1, WHITE);
        currentY += lineHeight;
    } else {
        Camera2D camera = cam.getCamera2D();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1);
        oss << "Position: (" << camera.target.x << ", " << camera.target.y << ")";
        DrawTextEx(font, oss.str().c_str(), {20, (float)currentY}, 16, 1, WHITE);
        currentY += lineHeight;
        oss.str("");
        oss << "Zoom: " << camera.zoom;
        DrawTextEx(font, oss.str().c_str(), {20, (float)currentY}, 16, 1, WHITE);
    }
}

void DebugSystem::drawTileInfo() {
    if (_selectedTileX >= 0 && _selectedTileY >= 0) {
        int infoX = GetScreenWidth() - 320;
        int infoY = 50;
        Font font = FontManager::getInstance().getFont("medium");
        DrawRectangleRounded((Rectangle){(float)infoX, (float)infoY, 300, 260}, 0.05f, 5, Fade(BLACK, 0.7f));
        DrawRectangleRoundedLines((Rectangle){(float)infoX, (float)infoY, 300, 260}, 0.05f, 5, BLACK);
        DrawTextEx(font, "TILE INFORMATION", {(float)(infoX + 10), (float)(infoY + 10)}, 18, 1, YELLOW);
        char posText[64];
        snprintf(posText, sizeof(posText), "Position: (%d, %d)", _selectedTileX - 1, _selectedTileY - 1);
        DrawTextEx(font, posText, {(float)(infoX + 10), (float)(infoY + 45)}, 18, 1, WHITE);
        
        GameWorld::Tile tile = GameWorld::getInstance().getTileAt(_selectedTileX, _selectedTileY);
        DrawTextEx(font, "Resources:", {(float)(infoX + 10), (float)(infoY + 80)}, 18, 1, SKYBLUE);
        
        char resourceText[64];
        snprintf(resourceText, sizeof(resourceText), "Food: %d", tile.resources.food);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 110)}, 18, 1, YELLOW);
        snprintf(resourceText, sizeof(resourceText), "Linemate: %d", tile.resources.linemate);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 130)}, 18, 1, ORANGE);
        snprintf(resourceText, sizeof(resourceText), "Deraumere: %d", tile.resources.deraumere);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 150)}, 18, 1, PURPLE);
        snprintf(resourceText, sizeof(resourceText), "Sibur: %d", tile.resources.sibur);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 170)}, 18, 1, SKYBLUE);
        snprintf(resourceText, sizeof(resourceText), "Mendiane: %d", tile.resources.mendiane);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 190)}, 18, 1, PINK);
        snprintf(resourceText, sizeof(resourceText), "Phiras: %d", tile.resources.phiras);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 210)}, 18, 1, LIME);
        snprintf(resourceText, sizeof(resourceText), "Thystame: %d", tile.resources.thystame);
        DrawTextEx(font, resourceText, {(float)(infoX + 10), (float)(infoY + 230)}, 18, 1, MAROON);
    }
}
