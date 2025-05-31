/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** TileInteraction
*/

#ifndef TILEINTERACTION_HPP_
#define TILEINTERACTION_HPP_

#include "raylib.h"
#include "GameWorld.hpp"
#include <vector>

class GameWorld;

class TileInteraction {
public:
    static TileInteraction& getInstance();
    void initialize();
    void update(GameWorld* world);
    void draw();
    bool getSelectedTile(int& x, int& y) const;
    void clearSelection();
private:
    TileInteraction() = default;
    ~TileInteraction() = default;
    TileInteraction(const TileInteraction&) = delete;
    TileInteraction& operator=(const TileInteraction&) = delete;
    bool _hasSelection = false;
    int _selectedX = -1;
    int _selectedY = -1;
    Color _highlightColor = YELLOW;
    float _pulseTimer = 0.0f;
    void checkTileClick(GameWorld* world);
    Ray getMouseRay(Camera camera);
    bool checkRayTileIntersection(Ray ray, int x, int y, float* distance);
    Vector2 screenToWorld2D(Vector2 screenPos, Camera2D camera);
};

#endif
