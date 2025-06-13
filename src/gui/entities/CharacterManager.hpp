/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CharacterManager
*/

#ifndef CHARACTERMANAGER_HPP_
    #define CHARACTERMANAGER_HPP_

#include "Character.hpp"
#include "raylib.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

struct ElevationParticle {
    Vector3 position;
    Vector3 velocity;
    float life;
    float maxLife;
    float size;
    Color color;
};

class CharacterManager {
public:
    static CharacterManager& getInstance();

    void initialize();
    void cleanup();
    void update(float dt);
    void draw3D(Camera camera);
    void draw2D(Camera2D camera);

    void addCharacter(int id, const Vector3& position, const std::string& team, int level = 1);
    void removeCharacter(int id);
    Character* getCharacter(int id);
    Character* getHoveredCharacter(Camera camera);

    void setSelectedCharacter(Character* character);
    Character* getSelectedCharacter() const { return _selectedCharacter; }

    void endAllElevations();
    void setTimeUnit(int timeUnit) { _timeUnit = timeUnit; }
    bool isModelLoaded() const { return _modelLoaded; }

private:
    CharacterManager() = default;
    ~CharacterManager();
    CharacterManager(const CharacterManager&) = delete;
    CharacterManager& operator=(const CharacterManager&) = delete;

    std::vector<std::unique_ptr<Character>> _characters;
    std::map<std::string, std::vector<Character*>> _tileCharacters;
    std::map<Character*, std::vector<ElevationParticle>> _elevationParticles;

    Model _characterModel;
    ModelAnimation* _animations = nullptr;
    int _animCount = 0;
    int _frameCounter = 0;
    bool _modelLoaded = false;

    float _particleTimer = 0.0f;
    float _animationTimer = 0.0f;
    int _timeUnit = 100;
    Character* _hoveredCharacter = nullptr;
    Character* _selectedCharacter = nullptr;

    void loadModel();
    void updateTilePositions();
    void updateElevationParticles(float dt);
    void createElevationParticles(Character* character);
    void drawCharacter(Character* character, Camera camera, bool isHovered, bool isSelected);
    void drawCharacterOutline(Character* character, Color color);
    void drawElevationParticles(Character* character, Camera camera);
    Color getTeamColor();
    std::string getTileKey(const Vector2& pos) const;
};

#endif
