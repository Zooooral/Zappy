/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** TileInteraction
*/

#include "TileInteraction.hpp"
#include "GameWorld.hpp"
#include "CameraController.hpp"
#include "DebugSystem.hpp"
#include "CharacterManager.hpp"
#include <raymath.h>
#include <cfloat>

TileInteraction& TileInteraction::getInstance() {
    static TileInteraction instance;
    return instance;
}

void TileInteraction::initialize() {
    _hasSelection = false;
    _selectedX = -1;
    _selectedY = -1;
    _pulseTimer = 0.0f;
}

void TileInteraction::update(GameWorld* world) {
    _pulseTimer += GetFrameTime();
    checkTileClick(world);
}

void TileInteraction::draw() {
    if (!_hasSelection) return;
    float pulse = (sinf(_pulseTimer * 3.0f) + 1.0f) * 0.5f;
    Color color = Fade(_highlightColor, 0.3f + pulse * 0.4f);
    CameraController& cam = CameraController::getInstance();
    if (cam.is3DMode()) {
        Vector3 pos = Vector3{ (float)_selectedX, 1.1f, (float)_selectedY };
        BeginMode3D(cam.getCamera3D());
        DrawCube(pos, 1.0f, 0.2f, 1.0f, color);
        DrawCubeWires(pos, 1.0f, 0.2f, 1.0f, _highlightColor);
        EndMode3D();
    } else {
        const float TILE_SIZE = 50.0f;
        Rectangle rect = {
            (float)(_selectedX - 1) * TILE_SIZE,
            (float)(_selectedY - 1) * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE
        };
        BeginMode2D(cam.getCamera2D());
        DrawRectangleRec(rect, color);
        DrawRectangleLinesEx(rect, 2.0f + pulse * 2.0f, _highlightColor);
        EndMode2D();
    }
}

bool TileInteraction::getSelectedTile(int& x, int& y) const {
    if (_hasSelection) {
        x = _selectedX;
        y = _selectedY;
        return true;
    }
    return false;
}

void TileInteraction::clearSelection() {
    _hasSelection = false;
    _selectedX = -1;
    _selectedY = -1;
    DebugSystem::getInstance().clearSelectedTile();
}

void TileInteraction::checkTileClick(GameWorld* world) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    
    CameraController& cam = CameraController::getInstance();
    
    if (cam.is3DMode()) {
        CharacterManager& charMgr = CharacterManager::getInstance();
        Character* clickedChar = charMgr.getHoveredCharacter(cam.getCamera3D());
        if (clickedChar) {
            return;
        }
    }
    
    if (cam.is3DMode()) {
        Ray ray = getMouseRay(cam.getCamera3D());
        float closestDistance = FLT_MAX;
        int closestX = -1, closestY = -1;
        for (int y = 1; y < world->getTotalHeight() - 1; y++) {
            for (int x = 1; x < world->getTotalWidth() - 1; x++) {
                float distance;
                if (checkRayTileIntersection(ray, x, y, &distance)) {
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestX = x;
                        closestY = y;
                    }
                }
            }
        }
        if (closestX >= 0 && closestY >= 0) {
            _hasSelection = true;
            _selectedX = closestX;
            _selectedY = closestY;
            DebugSystem::getInstance().setSelectedTile(closestX, closestY);
        }
    } else {
        Vector2 worldPos = screenToWorld2D(GetMousePosition(), cam.getCamera2D());
        const float TILE_SIZE = 50.0f;
        int x = (int)(worldPos.x / TILE_SIZE);
        int y = (int)(worldPos.y / TILE_SIZE);
        x += 1;
        y += 1;
        if (x >= 1 && x < world->getTotalWidth() - 1 && y >= 1 && y < world->getTotalHeight() - 1) {
            _hasSelection = true;
            _selectedX = x;
            _selectedY = y;
            DebugSystem::getInstance().setSelectedTile(x, y);
        }
    }
}

Ray TileInteraction::getMouseRay(Camera camera) {
    Vector2 mousePos = GetMousePosition();
    return GetMouseRay(mousePos, camera);
}

bool TileInteraction::checkRayTileIntersection(Ray ray, int x, int y, float* distance) {
    Vector3 tileMin = Vector3{ (float)x - 0.5f, -0.1f, (float)y - 0.5f };
    Vector3 tileMax = Vector3{ (float)x + 0.5f, 1.5f, (float)y + 0.5f };
    BoundingBox box = { tileMin, tileMax };
    RayCollision collision = GetRayCollisionBox(ray, box);
    if (collision.hit) {
        *distance = collision.distance;
        return true;
    }
    return false;
}

Vector2 TileInteraction::screenToWorld2D(Vector2 screenPos, Camera2D camera) {
    Vector2 worldPos;
    worldPos.x = (screenPos.x - camera.offset.x) / camera.zoom + camera.target.x;
    worldPos.y = (screenPos.y - camera.offset.y) / camera.zoom + camera.target.y;
    return worldPos;
}
