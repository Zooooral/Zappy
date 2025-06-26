/*
** EPITECH PROJECT, 2025
** src/gui/entities/CharacterManager.hpp
** File description:
** CharacterManager with proper const correctness
*/

#ifndef CHARACTERMANAGER_HPP_
#define CHARACTERMANAGER_HPP_

#include "Character.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>
#include "raylib.h"

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
    
    ~CharacterManager();
    CharacterManager(const CharacterManager&) = delete;
    CharacterManager& operator=(const CharacterManager&) = delete;
    CharacterManager(CharacterManager&&) = delete;
    CharacterManager& operator=(CharacterManager&&) = delete;

    void initialize();
    void cleanup();
    void update(float dt);
    void draw2D(Camera2D camera) const;
    void draw3D(Camera camera);

    void addCharacter(int id, const Vector3& position, const std::string& team, int level);
    void removeCharacter(int id);
    Character* getCharacter(int id) const;
    Character* getHoveredCharacter(Camera camera) const;
    
    void setSelectedCharacter(Character* character);
    Character* getSelectedCharacter() const { return _selectedCharacter; }
    
    void endAllElevations();
    void clearAllCharacters();
    float getTimeUnit() const { return _timeUnit; }
    void setTimeUnit(float timeUnit) { _timeUnit = timeUnit; }
    
    const std::vector<std::unique_ptr<Character>>& getCharacters() const { return _characters; }

    std::vector<Character*> getAllCharacters() const;
    Color getTeamColor(const std::string& teamName) const;

private:
    CharacterManager() = default;

    std::vector<std::unique_ptr<Character>> _characters;
    std::unordered_map<std::string, std::vector<Character*>> _tileCharacters;
    std::unordered_map<Character*, std::vector<ElevationParticle>> _elevationParticles;
    
    Character* _selectedCharacter = nullptr;
    mutable Character* _hoveredCharacter = nullptr;
    
    Model _characterModel;
    ModelAnimation* _animations = nullptr;
    int _animCount = 0;
    bool _modelLoaded = false;
    float _animationTimer = 0.0f;
    float _particleTimer = 0.0f;
    int _frameCounter = 0;
    float _timeUnit = 1.0f;
    
    mutable std::mutex _charactersMutex;

    void loadModel();
    void updateTilePositions();
    void updateElevationParticles(float dt);
    void createElevationParticles(Character* character);
    void drawElevationParticles(Character* character, Camera camera);
    
    void drawCharacter(Character* character, Camera camera, bool isHovered, bool isSelected) const;
    void drawCharacterOutline(Character* character, Color color) const;
    
    std::string getTileKey(const Vector2& pos) const;
};

#endif /* !CHARACTERMANAGER_HPP_ */
