/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Character
*/

#include "Character.hpp"
#include <cmath>

Character::Character(int id, const Vector3& position, const std::string& team, int level)
    : _id(id), _position(position), _displayPosition(position), _team(team), _level(level), _orientation(1), _isElevating(false) {
    _tilePosition = {std::floor(position.x), std::floor(position.z)};
}

void Character::setPosition(const Vector3& position) {
    _position = position;
    _tilePosition = {std::floor(position.x), std::floor(position.z)};
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

void Character::updateTilePosition(const Vector2& tilePos, int indexOnTile, int totalOnTile) {
    _tilePosition = tilePos;
    
    if (totalOnTile == 1) {
        _displayPosition = {tilePos.x, _position.y, tilePos.y};
        return;
    }
    
    float radius = 0.35f;
    float angle = (2.0f * PI * indexOnTile) / totalOnTile;
    
    _displayPosition.x = tilePos.x + radius * cosf(angle);
    _displayPosition.y = _position.y;
    _displayPosition.z = tilePos.y + radius * sinf(angle);
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
