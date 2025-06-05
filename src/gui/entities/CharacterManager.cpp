/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CharacterManager
*/

#include "CharacterManager.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>

CharacterManager& CharacterManager::getInstance() {
    static CharacterManager instance;
    return instance;
}

CharacterManager::~CharacterManager() {
    cleanup();
}

void CharacterManager::initialize() {
    loadModel();
}

void CharacterManager::cleanup() {
    if (_modelLoaded) {
        if (_animations) {
            for (int i = 0; i < _animCount; i++) {
                UnloadModelAnimation(_animations[i]);
            }
            RL_FREE(_animations);
            _animations = nullptr;
        }
        if (IsWindowReady() && _characterModel.meshCount > 0) {
            UnloadModel(_characterModel);
        }
        _modelLoaded = false;
    }
    _characters.clear();
    _tileCharacters.clear();
    _elevationParticles.clear();
}

void CharacterManager::update(float dt) {
    _particleTimer += dt;
    _animationTimer += dt;
    
    if (_modelLoaded && _animCount > 0) {
        _frameCounter++;
        if (_frameCounter >= _animations[0].frameCount) {
            _frameCounter = 0;
        }
        UpdateModelAnimation(_characterModel, _animations[0], _frameCounter);
    }
    
    for (auto& character : _characters) {
        character->updateMovement(dt, _timeUnit);
        if (character->isElevating()) {
            if (_particleTimer > 0.05f) {
                createElevationParticles(character.get());
                _particleTimer = 0.0f;
            }
        }
    }
    
    updateTilePositions();
    updateElevationParticles(dt);
}

void CharacterManager::updateElevationParticles(float dt) {
    for (auto& [character, particles] : _elevationParticles) {
        for (auto& particle : particles) {
            particle.life -= dt;
            particle.position.x += particle.velocity.x * dt;
            particle.position.y += particle.velocity.y * dt;
            particle.position.z += particle.velocity.z * dt;
            
            float lifeRatio = particle.life / particle.maxLife;
            particle.color.a = (unsigned char)(255 * lifeRatio);
            particle.size = 0.02f * lifeRatio;
        }
        
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                          [](const ElevationParticle& p) { return p.life <= 0.0f; }),
            particles.end()
        );
    }
    
    for (auto it = _elevationParticles.begin(); it != _elevationParticles.end();) {
        if (!it->first->isElevating() && it->second.empty()) {
            it = _elevationParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void CharacterManager::createElevationParticles(Character* character) {
    Vector3 charPos = character->getDisplayPosition();
    auto& particles = _elevationParticles[character];
    
    for (int i = 0; i < 3; i++) {
        ElevationParticle particle;
        
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float radius = GetRandomValue(2, 4) / 10.0f;
        float height = GetRandomValue(0, 5) / 10.0f;
        
        particle.position = {
            charPos.x + cosf(angle) * radius,
            charPos.y + 0.3f + height,
            charPos.z + sinf(angle) * radius
        };
        
        particle.velocity = {
            cosf(angle) * 0.2f,
            GetRandomValue(5, 15) / 100.0f,
            sinf(angle) * 0.2f
        };
        
        particle.life = particle.maxLife = GetRandomValue(8, 15) / 10.0f;
        particle.size = GetRandomValue(2, 8) / 100.0f;
        particle.color = {255, static_cast<unsigned char>(GetRandomValue(0, 50)), 0, 255};
        
        particles.push_back(particle);
    }
}

void CharacterManager::draw3D(Camera camera) {
    if (!_modelLoaded) return;
    
    _hoveredCharacter = nullptr;
    
    BeginMode3D(camera);
    
    for (auto& character : _characters) {
        bool isHovered = character->isMouseOver(camera);
        bool isSelected = (character.get() == _selectedCharacter);
        
        if (isHovered) {
            _hoveredCharacter = character.get();
        }
        
        drawCharacter(character.get(), camera, isHovered, isSelected);
        
        if (character->isElevating()) {
            drawElevationParticles(character.get(), camera);
        }
    }
    
    EndMode3D();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && _hoveredCharacter) {
        setSelectedCharacter(_hoveredCharacter);
    }
}

void CharacterManager::drawElevationParticles(Character* character, Camera) {
    auto it = _elevationParticles.find(character);
    if (it == _elevationParticles.end()) return;
    
    for (const auto& particle : it->second) {
        DrawSphere(particle.position, particle.size, particle.color);
    }
}

void CharacterManager::draw2D(Camera2D camera) {
    BeginMode2D(camera);
    
    const float TILE_SIZE = 50.0f;
    
    for (auto& [tileKey, charactersOnTile] : _tileCharacters) {
        if (charactersOnTile.empty()) continue;
        
        Vector2 baseTilePos = charactersOnTile[0]->getTilePosition();
            Vector2 screenPos = {
                (baseTilePos.x - 1) * TILE_SIZE + TILE_SIZE * 0.5f,
                (baseTilePos.y - 1) * TILE_SIZE + TILE_SIZE * 0.5f
            };
        
        int playerCount = charactersOnTile.size();
        
        for (int i = 0; i < playerCount; i++) {
            Character* character = charactersOnTile[i];
            bool isSelected = (character == _selectedCharacter);
            
            Vector2 playerPos = screenPos;
            if (playerCount > 1) {
                float angle = (2.0f * PI * i) / playerCount;
                float radius = 12.0f;
                playerPos.x += cosf(angle) * radius;
                playerPos.y += sinf(angle) * radius;
            }
            
            Color playerColor = BLUE;
            
            if (character->isElevating()) {
                float pulse = (sinf(_animationTimer * 8.0f) + 1.0f) * 0.5f;
                playerColor = ColorLerp(BLUE, GOLD, pulse * 0.6f);
                
                for (int j = 0; j < 6; j++) {
                    float sparkleAngle = (_animationTimer * 3.0f + j * 60.0f) * DEG2RAD;
                    float sparkleRadius = 18.0f + sinf(_animationTimer * 4.0f) * 4.0f;
                    Vector2 sparklePos = {
                        playerPos.x + cosf(sparkleAngle) * sparkleRadius,
                        playerPos.y + sinf(sparkleAngle) * sparkleRadius
                    };
                    DrawCircle(sparklePos.x, sparklePos.y, 2.0f, Fade(GOLD, 0.7f));
                }
            }
            
            if (isSelected) {
                DrawRing(playerPos, 14.0f, 16.0f, 0, 360, 32, Fade(YELLOW, 0.8f));
                DrawRing(playerPos, 12.0f, 14.0f, 0, 360, 32, Fade(WHITE, 0.4f));
            }
            
            Vector2 shadowOffset = {2.0f, 2.0f};
            Vector2 shadowPos = {playerPos.x + shadowOffset.x, playerPos.y + shadowOffset.y};
            DrawPoly(shadowPos, 6, 8.0f, 0.0f, Fade(BLACK, 0.3f));
            
            DrawPoly(playerPos, 6, 10.0f, _animationTimer * 10.0f, playerColor);
            DrawPolyLines(playerPos, 6, 10.0f, _animationTimer * 10.0f, Fade(WHITE, 0.8f));
        }
    }
    
    EndMode2D();
}

void CharacterManager::addCharacter(int id, const Vector3& position, const std::string& team, int level) {
    auto character = std::make_unique<Character>(id, position, team, level);
    _characters.push_back(std::move(character));
}

void CharacterManager::removeCharacter(int id) {
    auto it = std::find_if(_characters.begin(), _characters.end(),
        [id](const std::unique_ptr<Character>& c) { return c->getId() == id; });
    
    if (it != _characters.end()) {
        if (it->get() == _selectedCharacter) {
            _selectedCharacter = nullptr;
        }
        _elevationParticles.erase(it->get());
        _characters.erase(it);
    }
}

Character* CharacterManager::getCharacter(int id) {
    auto it = std::find_if(_characters.begin(), _characters.end(),
        [id](const std::unique_ptr<Character>& c) { return c->getId() == id; });
    return (it != _characters.end()) ? it->get() : nullptr;
}

Character* CharacterManager::getHoveredCharacter(Camera camera) {
    for (auto& character : _characters) {
        if (character->isMouseOver(camera)) {
            return character.get();
        }
    }
    return nullptr;
}

void CharacterManager::setSelectedCharacter(Character* character) {
    _selectedCharacter = character;
}

void CharacterManager::endAllElevations() {
    for (auto& character : _characters) {
        if (character->isElevating()) {
            character->setElevating(false);
        }
    }
}

void CharacterManager::loadModel() {
    const char* characterPath = "assets/characters/character.glb";
    if (FileExists(characterPath)) {
        _characterModel = LoadModel(characterPath);
        _modelLoaded = true;
        _animations = LoadModelAnimations(characterPath, &_animCount);
        std::cout << "INFO: Character model loaded successfully" << std::endl;
    } else {
        std::cerr << "ERROR: Character model not found at " << characterPath << std::endl;
    }
}

void CharacterManager::updateTilePositions() {
    _tileCharacters.clear();
    
    for (auto& character : _characters) {
        Vector2 tilePos = character->getTilePosition();
        std::string key = getTileKey(tilePos);
        _tileCharacters[key].push_back(character.get());
    }
    
    for (auto& [key, charactersOnTile] : _tileCharacters) {
        if (!charactersOnTile.empty()) {
            Vector2 tilePos = charactersOnTile[0]->getTilePosition();
            for (size_t i = 0; i < charactersOnTile.size(); ++i) {
                charactersOnTile[i]->updateTilePosition(tilePos, static_cast<int>(i), static_cast<int>(charactersOnTile.size()));
            }
        }
    }
}

void CharacterManager::drawCharacter(Character* character, Camera camera, bool isHovered, bool isSelected) {
    Vector3 position = character->getDisplayPosition();
    Color tint = WHITE;
    
    float scale = 0.000025f;
    position.y -= 0.16f;
    Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
        float rotationAngle = character->getRotationAngle();
        DrawModelEx(_characterModel, position, rotationAxis, rotationAngle, Vector3{scale, scale, scale}, tint);
    if (isHovered) {
        drawCharacterOutline(character, YELLOW);
    }
    if (isSelected) {
        drawCharacterOutline(character, ORANGE);
    }
    
    Vector3 levelPos = {position.x, position.y + 1.2f, position.z};
    Vector2 screenPos = GetWorldToScreen(levelPos, camera);
    DrawText(("Lv." + std::to_string(character->getLevel())).c_str(),
             static_cast<int>(screenPos.x - 15), static_cast<int>(screenPos.y), 16, WHITE);
}

void CharacterManager::drawCharacterOutline(Character* character, Color color) {
    BoundingBox bbox = character->getBoundingBox();
    DrawBoundingBox(bbox, color);
}

Color CharacterManager::getTeamColor() {
    return Color{79, 122, 232, 255};
}

std::string CharacterManager::getTileKey(const Vector2& pos) const {
    std::ostringstream oss;
    oss << static_cast<int>(pos.x) << "," << static_cast<int>(pos.y);
    return oss.str();
}
