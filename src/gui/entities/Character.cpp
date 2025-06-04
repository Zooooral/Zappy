/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Character
*/

#include "Character.hpp"
#include <cmath>
#include <raymath.h>

Character::Character(int id, const Vector3& position, const std::string& team, int level)
    : _id(id), _position(position), _targetPosition(position), _displayPosition(position), 
      _team(team), _level(level), _orientation(1), _isElevating(false), _isMoving(false), 
      _moveProgress(0.0f), _moveStartPos(position) {
    _tilePosition = {std::floor(position.x), std::floor(position.z)};
}

void Character::setPosition(const Vector3& position) {
    if (!_isMoving) {
        _position = position;
        _targetPosition = position;
        _displayPosition = position;
        _tilePosition = {std::floor(position.x), std::floor(position.z)};
    }
}

void Character::setTargetPosition(const Vector3& target) {
    if (Vector3Distance(_position, target) > 0.1f) {
        _moveStartPos = _position;
        _targetPosition = target;
        _isMoving = true;
        _moveProgress = 0.0f;
    }
}

void Character::setLevel(int level) {
    _level = level;
}

void Character::setOrientation(int orientation) {
    _orientation = orientation;
}

void Character::setInventory(const CharacterInventory& inventory) {
    _inventory = inventory;
}

void Character::updateMovement(float dt, float timeUnit) {
    if (_isMoving) {
        float moveSpeed = BASE_MOVE_SPEED * (timeUnit / 100.0f);
        _moveProgress += dt * moveSpeed;
        
        if (_moveProgress >= 1.0f) {
            _moveProgress = 1.0f;
            _isMoving = false;
            _position = _targetPosition;
        }
        
        _position = Vector3Lerp(_moveStartPos, _targetPosition, _moveProgress);
        _tilePosition = {std::floor(_position.x), std::floor(_position.z)};
    }
}

void Character::updateTilePosition(const Vector2& tilePos, int indexOnTile, int totalOnTile) {
    _tilePosition = tilePos;
    
    Vector3 basePos = _position;
    
    if (totalOnTile == 1) {
        _displayPosition = {basePos.x, basePos.y, basePos.z};
        return;
    }
    
    float radius = 0.35f;
    float angle = (2.0f * PI * indexOnTile) / totalOnTile;
    
    _displayPosition.x = basePos.x + radius * cosf(angle);
    _displayPosition.y = basePos.y;
    _displayPosition.z = basePos.z + radius * sinf(angle);
}

float Character::getRotationAngle() const {
    switch (_orientation) {
        case 1: return 0.0f;
        case 2: return 90.0f;
        case 3: return 180.0f;
        case 4: return 270.0f;
        default: return 0.0f;
    }
}

bool Character::isMouseOver(Camera camera) const {
    Vector2 mousePos = GetMousePosition();
    Ray mouseRay = GetMouseRay(mousePos, camera);
    BoundingBox bbox = getBoundingBox();
    RayCollision collision = GetRayCollisionBox(mouseRay, bbox);
    return collision.hit;
}

BoundingBox Character::getBoundingBox() const {
    Vector3 min = {
        _displayPosition.x - CHARACTER_SIZE / 2,
        _displayPosition.y,
        _displayPosition.z - CHARACTER_SIZE / 2
    };
    Vector3 max = {
        _displayPosition.x + CHARACTER_SIZE / 2,
        _displayPosition.y + CHARACTER_HEIGHT,
        _displayPosition.z + CHARACTER_SIZE / 2
    };
    return {min, max};
}
