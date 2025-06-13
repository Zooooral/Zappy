/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Character
*/

#ifndef CHARACTER_HPP_
    #define CHARACTER_HPP_

#include "raylib.h"
#include <string>
#include <unordered_map>

struct CharacterInventory {
    int food = 0;
    int linemate = 0;
    int deraumere = 0;
    int sibur = 0;
    int mendiane = 0;
    int phiras = 0;
    int thystame = 0;
};

class Character {
public:
    Character(int id, const Vector3& position, const std::string& team, int level = 1);
    ~Character() = default;

    void setPosition(const Vector3& position);
    void setTargetPosition(const Vector3& target);
    void setLevel(int level);
    void setOrientation(int orientation);
    void setInventory(const CharacterInventory& inventory);
    void updateTilePosition(const Vector2& tilePos, int indexOnTile, int totalOnTile);
    void setElevating(bool elevating) { _isElevating = elevating; }
    void updateMovement(float dt, float timeUnit);

    int getId() const { return _id; }
    Vector3 getPosition() const { return _position; }
    Vector3 getDisplayPosition() const { return _displayPosition; }
    std::string getTeam() const { return _team; }
    int getLevel() const { return _level; }
    int getOrientation() const { return _orientation; }
    float getRotationAngle() const;
    const CharacterInventory& getInventory() const { return _inventory; }
    Vector2 getTilePosition() const { return _tilePosition; }
    bool isElevating() const { return _isElevating; }
    bool isMoving() const { return _isMoving; }
    
    bool isMouseOver(Camera camera) const;
    BoundingBox getBoundingBox() const;

private:
    int _id;
    Vector3 _position;
    Vector3 _targetPosition;
    Vector3 _displayPosition;
    Vector2 _tilePosition;
    std::string _team;
    int _level;
    int _orientation;
    CharacterInventory _inventory;
    bool _isElevating;
    bool _isMoving;
    float _moveProgress;
    Vector3 _moveStartPos;
    
    static constexpr float CHARACTER_SIZE = 0.5f;
    static constexpr float CHARACTER_HEIGHT = 1.0f;
    static constexpr float BASE_MOVE_SPEED = 2.0f;
};

#endif
