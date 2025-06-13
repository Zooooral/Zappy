/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CameraController
*/

#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <cmath>

#include "CameraController.hpp"
#include "GameWorld.hpp"
#include "ConfigManager.hpp"

CameraController& CameraController::getInstance() {
    static CameraController instance;
    return instance;
}

void CameraController::initialize() {
    _camera3D.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    _camera3D.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    _camera3D.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    _camera3D.fovy = 45.0f;
    _camera3D.projection = CAMERA_PERSPECTIVE;
    _camera2D.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    _camera2D.target = (Vector2){ 0.0f, 0.0f };
    _camera2D.rotation = 0.0f;
    _camera2D.zoom = 1.0f;
}

void CameraController::update(float dt, GameWorld* world) {
    (void)world;
    processKeyboardInput(dt);
    processMouseInput();
    handleMouseDrag();
    if (_is3DMode) {
        constrainCamera3D();
    } else {
        constrainCamera2D();
    }
}

void CameraController::handleMouseDrag() {
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && _is3DMode) {
        _rotating = true;
        _lastMousePos = mousePos;
    }
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
        _rotating = false;
    }
    if (_rotating && _is3DMode && (_lastMousePos.x != mousePos.x || _lastMousePos.y != mousePos.y)) {
        Vector2 delta = Vector2Subtract(mousePos, _lastMousePos);
        float horizontalAngle = -delta.x * 0.005f;
        Vector3 toTarget = Vector3Subtract(_camera3D.position, _camera3D.target);
        float distance = Vector3Length(toTarget);
        float currentRadius = distance;
        float currentPhi = atan2f(toTarget.x, toTarget.z);
        float currentTheta = acosf(toTarget.y / distance);
        currentPhi += horizontalAngle;
        currentTheta += delta.y * 0.005f;
        currentTheta = fmaxf(0.1f, fminf(PI - 0.1f, currentTheta));
        _camera3D.position.x = _camera3D.target.x + currentRadius * sinf(currentTheta) * sinf(currentPhi);
        _camera3D.position.y = _camera3D.target.y + currentRadius * cosf(currentTheta);
        _camera3D.position.z = _camera3D.target.z + currentRadius * sinf(currentTheta) * cosf(currentPhi);
        _lastMousePos = mousePos;
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        _dragging = true;
        _lastMousePos = mousePos;
    }
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        _dragging = false;
    }
    if (_dragging && (_lastMousePos.x != mousePos.x || _lastMousePos.y != mousePos.y)) {
        Vector2 delta = Vector2Subtract(mousePos, _lastMousePos);
        if (_is3DMode) {
            float sensitivity = 0.005f;
            Vector3 forward = Vector3Subtract(_camera3D.target, _camera3D.position);
            Vector3 right = Vector3CrossProduct(forward, _camera3D.up);
            right = Vector3Normalize(right);
            Vector3 moveHorizontal = Vector3Scale(right, -delta.x * sensitivity);
            Vector3 moveVertical = Vector3Scale(_camera3D.up, delta.y * sensitivity);
            Vector3 totalMove = Vector3Add(moveHorizontal, moveVertical);
            _camera3D.position = Vector3Add(_camera3D.position, totalMove);
            _camera3D.target = Vector3Add(_camera3D.target, totalMove);
        } else {
            float sensitivity = 1.0f / _camera2D.zoom;
            _camera2D.target.x -= delta.x * sensitivity;
            _camera2D.target.y -= delta.y * sensitivity;
        }
        _lastMousePos = mousePos;
    }
}

void CameraController::setConstraints(float minZoom, float maxZoom, float worldWidth, float worldHeight) {
    _minZoom = minZoom;
    _maxZoom = maxZoom;
    _worldWidth = worldWidth;
    _worldHeight = worldHeight;
}

void CameraController::resetCamera() {
    if (_is3DMode) {
        _camera3D.position = (Vector3){ _worldWidth / 2.0f, 10.0f, _worldHeight / 2.0f + 10.0f };
        _camera3D.target = (Vector3){ _worldWidth / 2.0f, 0.0f, _worldHeight / 2.0f };
    } else {
        _camera2D.target = (Vector2){ (_worldWidth - 2) * 25.0f, (_worldHeight - 2) * 25.0f };
        _camera2D.zoom = 1.0f;
    }
}

void CameraController::toggleViewMode() {
    _is3DMode = !_is3DMode;
    if (!_is3DMode) {
        _camera2D.target.x = (_camera3D.target.x - 1) * 50.0f;
        _camera2D.target.y = (_camera3D.target.z - 1) * 50.0f;
    }
}

void CameraController::constrainCamera3D() {
    float minHeight = 0.5f;
    if (_camera3D.position.y < minHeight) {
        _camera3D.position.y = minHeight;
    }
    float maxHeight = 50.0f;
    if (_camera3D.position.y > maxHeight) {
        _camera3D.position.y = maxHeight;
    }
    float maxDistance = std::max(_worldWidth, _worldHeight) * 2.0f;
    float currentDistance = Vector3Length(Vector3Subtract(_camera3D.position, _camera3D.target));
    if (currentDistance > maxDistance) {
        Vector3 direction = Vector3Normalize(Vector3Subtract(_camera3D.position, _camera3D.target));
        _camera3D.position = Vector3Add(_camera3D.target, Vector3Scale(direction, maxDistance));
    }
    float minDistance = 1.0f;
    if (currentDistance < minDistance) {
        Vector3 direction = Vector3Normalize(Vector3Subtract(_camera3D.position, _camera3D.target));
        _camera3D.position = Vector3Add(_camera3D.target, Vector3Scale(direction, minDistance));
    }
}

void CameraController::constrainCamera2D() {
    _camera2D.zoom = std::max(_minZoom, std::min(_maxZoom, _camera2D.zoom));
    float padding = 100.0f;
    float minX = -padding;
    float minY = -padding;
    float maxX = (_worldWidth - 2) * 50.0f + padding;
    float maxY = (_worldHeight - 2) * 50.0f + padding;
    _camera2D.target.x = std::max(minX, std::min(maxX, _camera2D.target.x));
    _camera2D.target.y = std::max(minY, std::min(maxY, _camera2D.target.y));
}

void CameraController::processKeyboardInput(float dt) {
    const float MOVE_SPEED = 10.0f;
    const float FAST_SPEED = 20.0f;
    ConfigManager& config = ConfigManager::getInstance();
    int forwardKey = config.getKeyBinding("Move Forward");
    int backwardKey = config.getKeyBinding("Move Backward");
    int leftKey = config.getKeyBinding("Move Left");
    int rightKey = config.getKeyBinding("Move Right");
    int toggleKey = config.getKeyBinding("Toggle View");
    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? FAST_SPEED : MOVE_SPEED;
    if (IsKeyPressed(toggleKey)) {
        toggleViewMode();
    }
    if (_is3DMode) {
        Vector3 forward = Vector3Normalize(Vector3Subtract(_camera3D.target, _camera3D.position));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, _camera3D.up));
        if (IsKeyDown(forwardKey)) {
            Vector3 move = Vector3Scale(forward, speed * dt);
            _camera3D.position = Vector3Add(_camera3D.position, move);
            _camera3D.target = Vector3Add(_camera3D.target, move);
        }
        if (IsKeyDown(backwardKey)) {
            Vector3 move = Vector3Scale(forward, -speed * dt);
            _camera3D.position = Vector3Add(_camera3D.position, move);
            _camera3D.target = Vector3Add(_camera3D.target, move);
        }
        if (IsKeyDown(leftKey)) {
            Vector3 move = Vector3Scale(right, -speed * dt);
            _camera3D.position = Vector3Add(_camera3D.position, move);
            _camera3D.target = Vector3Add(_camera3D.target, move);
        }
        if (IsKeyDown(rightKey)) {
            Vector3 move = Vector3Scale(right, speed * dt);
            _camera3D.position = Vector3Add(_camera3D.position, move);
            _camera3D.target = Vector3Add(_camera3D.target, move);
        }
    } else {
        float moveSpeed = speed * dt * 50.0f / _camera2D.zoom;
        if (IsKeyDown(forwardKey)) _camera2D.target.y -= moveSpeed;
        if (IsKeyDown(backwardKey)) _camera2D.target.y += moveSpeed;
        if (IsKeyDown(leftKey)) _camera2D.target.x -= moveSpeed;
        if (IsKeyDown(rightKey)) _camera2D.target.x += moveSpeed;
    }
}

void CameraController::processMouseInput() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        if (_is3DMode) {
            float zoomSpeed = 1.0f;
            Vector3 direction = Vector3Normalize(Vector3Subtract(_camera3D.target, _camera3D.position));
            Vector3 move = Vector3Scale(direction, wheel * zoomSpeed);
            Vector3 newPos = Vector3Add(_camera3D.position, move);
            float newDistance = Vector3Length(Vector3Subtract(newPos, _camera3D.target));
            if (newDistance >= 1.0f && newDistance <= 50.0f) {
                _camera3D.position = newPos;
            }
        } else {
            float zoomSpeed = 0.1f;
            _camera2D.zoom += wheel * zoomSpeed;
            _camera2D.zoom = std::max(_minZoom, std::min(_maxZoom, _camera2D.zoom));
        }
    }
}
