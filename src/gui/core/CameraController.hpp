/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CameraController
*/

#ifndef CAMERACONTROLLER_HPP_
    #define CAMERACONTROLLER_HPP_

#include "raylib.h"

class GameWorld;

class CameraController {
public:
    static CameraController& getInstance();
    void initialize();
    void update(float dt, GameWorld* world);
    void handleMouseDrag();
    void setConstraints(float minZoom, float maxZoom, float worldWidth, float worldHeight);
    void resetCamera();
    void toggleViewMode();
    Camera getCamera3D() const { return _camera3D; }
    Camera2D getCamera2D() const { return _camera2D; }
    bool is3DMode() const { return _is3DMode; }
private:
    CameraController() = default;
    ~CameraController() = default;
    CameraController(const CameraController&) = delete;
    CameraController& operator=(const CameraController&) = delete;
    Camera _camera3D;
    Camera2D _camera2D;
    bool _is3DMode = true;
    float _minZoom = 0.2f;
    float _maxZoom = 5.0f;
    float _worldWidth = 0.0f;
    float _worldHeight = 0.0f;
    bool _dragging = false;
    bool _rotating = false;
    Vector2 _lastMousePos;
    void constrainCamera3D();
    void constrainCamera2D();
    void processKeyboardInput(float dt);
    void processMouseInput();
};

#endif
