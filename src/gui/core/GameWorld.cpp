/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameWorld
*/

#include "GameWorld.hpp"
#include "CameraController.hpp"
#include "DebugSystem.hpp"
#include "TileInteraction.hpp"
#include "Environment.hpp"
#include "FontManager.hpp"
#include <iostream>
#include <cmath>
#include "rlgl.h"

GameWorld& GameWorld::getInstance() {
    static GameWorld instance;
    return instance;
}

GameWorld::GameWorld() {
}

GameWorld::~GameWorld() {
    unloadResources();
}

void GameWorld::initialize(int playableWidth, int playableHeight) {
    _playableWidth = playableWidth;
    _playableHeight = playableHeight;
    _totalWidth = playableWidth + 2;
    _totalHeight = playableHeight + 2;

    generateMap(playableWidth, playableHeight);
    loadResources();
    loadResourceModels();
    
    PropManager::getInstance().initialize();
    generateProps();
    
    CameraController& cam = CameraController::getInstance();
    cam.initialize();
    cam.setConstraints(0.2f, 5.0f, _totalWidth, _totalHeight);
    cam.resetCamera();
    DebugSystem::getInstance().initialize();
    TileInteraction::getInstance().initialize();
    Environment::getInstance().initialize();
}

void GameWorld::updateTileResources(int x, int y, const TileResources& resources) {
    if (x >= 0 && x < _totalWidth && y >= 0 && y < _totalHeight) {
        _tileMap[y][x].resources = resources;
    }
}

Vector3 GameWorld::getResourcePosition(int index, Vector3 basePos) const {
    const Vector3 positions[] = {
        {basePos.x - 0.35f, basePos.y, basePos.z - 0.35f},
        {basePos.x + 0.35f, basePos.y, basePos.z - 0.35f},
        {basePos.x + 0.35f, basePos.y, basePos.z + 0.35f},
        {basePos.x - 0.35f, basePos.y, basePos.z + 0.35f},
        {basePos.x, basePos.y, basePos.z - 0.4f},
        {basePos.x + 0.4f, basePos.y, basePos.z},
        {basePos.x, basePos.y, basePos.z + 0.4f},
        {basePos.x - 0.4f, basePos.y, basePos.z}
    };
    
    return positions[index % 8];
}

void GameWorld::generateMap(int playableWidth, int playableHeight) {
    int totalWidth = playableWidth + 2;
    int totalHeight = playableHeight + 2;
    _tileMap.clear();
    _tileMap.resize(totalHeight, std::vector<Tile>(totalWidth));

    for (int y = 0; y < totalHeight; y++) {
        for (int x = 0; x < totalWidth; x++) {
            TileType tileType;
            float rotation = 0.0f;
            if (isCorner(x, y)) {
                tileType = TileType::CLIFF_CORNER_ROCK;
                rotation = getTileRotation(x, y, tileType);
            } else if (isEdge(x, y)) {
                tileType = TileType::CLIFF_ROCK;
                rotation = getTileRotation(x, y, tileType);
            } else {
                tileType = TileType::GROUND_GRASS;
                rotation = 0.0f;
            }
            _tileMap[y][x] = Tile(tileType, rotation);
        }
    }
}

void GameWorld::generateProps() {
    PropManager& propManager = PropManager::getInstance();
    
    for (int y = 1; y < _totalHeight - 1; y++) {
        for (int x = 1; x < _totalWidth - 1; x++) {
            if (_tileMap[y][x].type == TileType::GROUND_GRASS) {
                propManager.generatePropsForTile(_tileMap[y][x].props);
            }
        }
    }
}

void GameWorld::update(float dt) {
    CameraController::getInstance().update(dt, this);
    DebugSystem::getInstance().update(dt);
    TileInteraction::getInstance().update(this);
    Environment::getInstance().update(dt);
}

void GameWorld::draw() {
    CameraController& cam = CameraController::getInstance();
    if (cam.is3DMode()) {
        draw3D();
    } else {
        draw2D();
    }
    TileInteraction::getInstance().draw();
    DebugSystem::getInstance().draw();
}

void GameWorld::draw2D() {
    ClearBackground(Color{25, 25, 35, 255});
    CameraController& cam = CameraController::getInstance();
    BeginMode2D(cam.getCamera2D());

    const float TILE_SIZE = 50.0f;

    for (int y = 1; y < _totalHeight - 1; y++) {
        for (int x = 1; x < _totalWidth - 1; x++) {
            drawTile2D(x - 1, y - 1, _tileMap[y][x]);
        }
    }

    for (int x = 0; x <= _playableWidth; x++) {
        Color gridColor = x % 5 == 0 ? Fade(Color{100, 255, 200, 255}, 0.3f) : Fade(Color{70, 130, 180, 255}, 0.15f);
        float lineWidth = x % 5 == 0 ? 2.0f : 1.0f;
        DrawLineEx(
            Vector2{(float)(x * TILE_SIZE), 0},
            Vector2{(float)(x * TILE_SIZE), _playableHeight * TILE_SIZE},
            lineWidth, gridColor
        );
    }

    for (int y = 0; y <= _playableHeight; y++) {
        Color gridColor = y % 5 == 0 ? Fade(Color{100, 255, 200, 255}, 0.3f) : Fade(Color{70, 130, 180, 255}, 0.15f);
        float lineWidth = y % 5 == 0 ? 2.0f : 1.0f;
        DrawLineEx(
            Vector2{0, (float)(y * TILE_SIZE)},
            Vector2{_playableWidth * TILE_SIZE, (float)(y * TILE_SIZE)},
            lineWidth, gridColor
        );
    }

    EndMode2D();
    
    drawModernUI();
}

void GameWorld::drawModernUI() {
    char mapInfo[64];
    Font font = FontManager::getInstance().getFont("medium");
    snprintf(mapInfo, sizeof(mapInfo), "Map: %dx%d", _playableWidth, _playableHeight);
    
    DrawTextEx(font, "2D TACTICAL VIEW", {20, 15}, 28, 1, Color{100, 255, 200, 255});
    DrawTextEx(font, "Press P to switch to 3D view", {20, 45}, 16, 1, Color{150, 200, 255, 200});
    DrawTextEx(font, mapInfo, {20, 65}, 16, 1, Color{150, 200, 255, 200});
}

void GameWorld::draw3D() {
    ClearBackground(BLACK);
    CameraController& cam = CameraController::getInstance();
    Camera camera = cam.getCamera3D();

    Environment::getInstance().drawSky(camera);

    BeginMode3D(camera);

    Environment::getInstance().drawSea(camera);

    if (DebugSystem::getInstance().showWireframe()) {
        rlEnableWireMode();
    }

    for (int y = 0; y < _totalHeight; y++) {
        for (int x = 0; x < _totalWidth; x++) {
            drawTile3D(x, y, _tileMap[y][x]);
        }
    }

    if (DebugSystem::getInstance().showWireframe()) {
        rlDisableWireMode();
    }

    float playableX1 = 0.5f;
    float playableX2 = (float)_totalWidth - 0.5f;
    float playableZ1 = 0.5f;
    float playableZ2 = (float)_totalHeight - 0.5f;
    DrawLine3D({ playableX1, 0.0f, playableZ1 }, { playableX2, 0.0f, playableZ1 }, YELLOW);
    DrawLine3D({ playableX1, 0.0f, playableZ2 }, { playableX2, 0.0f, playableZ2 }, YELLOW);
    DrawLine3D({ playableX1, 0.0f, playableZ1 }, { playableX1, 0.0f, playableZ2 }, YELLOW);
    DrawLine3D({ playableX2, 0.0f, playableZ1 }, { playableX2, 0.0f, playableZ2 }, YELLOW);

    if (DebugSystem::getInstance().showDebugSpheres()) {
        DrawSphere(Vector3{0, 0, 0}, 0.1f, GREEN);
        DrawLine3D({0, 0, 0}, {1, 0, 0}, RED);
        DrawLine3D({0, 0, 0}, {0, 1, 0}, GREEN);
        DrawLine3D({0, 0, 0}, {0, 0, 1}, BLUE);
    }

    EndMode3D();

    Font font = FontManager::getInstance().getFont("medium");
    DrawTextEx(font, "Press P to switch to 2D view", {10, 10}, 18, 1, LIGHTGRAY);
    DrawTextEx(font, "F2: Debug Menu, C: Toggle Chat", {10, 40}, 18, 1, LIGHTGRAY);
    char mapInfo[64];
    snprintf(mapInfo, sizeof(mapInfo), "Map: %dx%d (total: %dx%d)", 
             _playableWidth, _playableHeight, _totalWidth, _totalHeight);
    DrawTextEx(font, mapInfo, {10, 70}, 18, 1, LIGHTGRAY);
}

void GameWorld::drawTile2D(int x, int y, const Tile& tile) {
    const float TILE_SIZE = 50.0f;

    Vector2 center = {
        (float)x * TILE_SIZE + TILE_SIZE / 2.0f,
        (float)y * TILE_SIZE + TILE_SIZE / 2.0f
    };

    Rectangle tileRect = {
        (float)x * TILE_SIZE,
        (float)y * TILE_SIZE,
        TILE_SIZE,
        TILE_SIZE
    };
    
    if (tile.type == TileType::GROUND_GRASS) {
        Color baseColor = Color{34, 139, 34, 255};
        Color shadowColor = Color{20, 80, 20, 255};
        
        Rectangle shadowRect = {tileRect.x + 2, tileRect.y + 2, tileRect.width, tileRect.height};
        DrawRectangleRounded(shadowRect, 0.1f, 8, shadowColor);
        DrawRectangleRounded(tileRect, 0.1f, 8, baseColor);
        DrawRectangleRoundedLines(tileRect, 0.1f, 8, Color{50, 200, 50, 150});
        
        drawTileResources2D(x + 1, y + 1, tile.resources, center, TILE_SIZE);
    } else {
        Color tileColor = getColorForTileType(tile.type);
        DrawRectangleRounded(tileRect, 0.05f, 6, tileColor);
        DrawRectangleRoundedLines(tileRect, 0.05f, 6, Fade(GRAY, 0.7f));
    }
}

void GameWorld::drawTileResources2D(int, int, const TileResources& resources, Vector2 center, float tileSize) {
    std::vector<std::pair<int, Color>> resourceList;
    
    if (resources.food > 0) {
        resourceList.push_back({resources.food, Color{255, 223, 0, 255}});
    }
    if (resources.linemate > 0) {
        resourceList.push_back({resources.linemate, Color{255, 140, 0, 255}});
    }
    if (resources.deraumere > 0) {
        resourceList.push_back({resources.deraumere, Color{148, 0, 211, 255}});
    }
    if (resources.sibur > 0) {
        resourceList.push_back({resources.sibur, Color{135, 206, 235, 255}});
    }
    if (resources.mendiane > 0) {
        resourceList.push_back({resources.mendiane, Color{255, 192, 203, 255}});
    }
    if (resources.phiras > 0) {
        resourceList.push_back({resources.phiras, Color{50, 205, 50, 255}});
    }
    if (resources.thystame > 0) {
        resourceList.push_back({resources.thystame, Color{128, 0, 0, 255}});
    }
    if (resources.eggs > 0) {
        resourceList.push_back({resources.eggs, Color{255, 255, 255, 255}});
    }
    
    int resourceCount = resourceList.size();
    if (resourceCount == 0) return;
    
    float resourceRadius = tileSize * 0.25f;
    for (int i = 0; i < resourceCount; i++) {
        float angle = (2.0f * PI * i) / resourceCount;
        float x = center.x + cosf(angle) * resourceRadius;
        float y = center.y + sinf(angle) * resourceRadius;
        
        DrawCircle(x + 1, y + 1, 3.0f, Fade(BLACK, 0.3f));
        DrawCircle(x, y, 3.0f, resourceList[i].second);
    }
}

void GameWorld::drawTile3D(int x, int y, const Tile& tile) {
    Vector3 originalPosition = getTilePosition3D(x, y);
    Vector3 position = originalPosition;
    Color tileColor = getColorForTileType(tile.type);
    Model* model = getModelForTileType(tile.type);
    
    if (model && model->meshCount > 0) {
        Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
        Vector3 scale = { 1.0f, 1.0f, 1.0f };
        if (tile.type == TileType::GROUND_GRASS) {
            position.y = 1.0f;
        }
        if (DebugSystem::getInstance().showWireframe()) {
            DrawModelWires(*model, position, 1.0f, WHITE);
        } else {
            DrawModelEx(*model, position, rotationAxis, tile.rotation, scale, WHITE);
        }
        if (DebugSystem::getInstance().showDebugSpheres() && tile.type == TileType::GROUND_GRASS) {
            DrawSphere(Vector3{position.x, position.y + 0.05f, position.z}, 0.05f, RED);
        }
    } else {
        float height = 0.1f;
        Vector3 cubeSize = { 1.0f, height, 1.0f };
        if (tile.type == TileType::CLIFF_ROCK || tile.type == TileType::CLIFF_CORNER_ROCK) {
            height = 0.5f;
            cubeSize.y = height;
            position.y += height / 2.0f;
        } else if (tile.type == TileType::GROUND_GRASS) {
            height = 0.3f;
            cubeSize.y = height;
            position.y = height / 2.0f;
        }
        if (DebugSystem::getInstance().showWireframe()) {
            DrawCubeWires(position, cubeSize.x, cubeSize.y, cubeSize.z, tileColor);
        } else {
            DrawCube(position, cubeSize.x, cubeSize.y, cubeSize.z, tileColor);
            DrawCubeWires(position, cubeSize.x, cubeSize.y, cubeSize.z, Fade(BLACK, 0.3f));
        }
        if (DebugSystem::getInstance().showDebugSpheres()) {
            Color sphereColor = (tile.type == TileType::GROUND_GRASS) ? GREEN : MAGENTA;
            DrawSphere(Vector3{position.x, position.y + height/2 + 0.05f, position.z}, 0.05f, sphereColor);
        }
        if (tile.rotation != 0.0f) {
            Vector3 direction = {
                cosf(tile.rotation * DEG2RAD) * 0.4f,
                0.0f,
                sinf(tile.rotation * DEG2RAD) * 0.4f
            };
            Vector3 lineEnd = {
                position.x + direction.x,
                position.y + height + 0.1f,
                position.z + direction.z
            };
            Vector3 lineStart = {
                position.x,
                position.y + height + 0.1f,
                position.z
            };
            DrawLine3D(lineStart, lineEnd, YELLOW);
            DrawSphere(lineEnd, 0.05f, YELLOW);
        }
    }
    
    if (tile.type == TileType::GROUND_GRASS) {
        PropManager& propManager = PropManager::getInstance();
        bool wireframe = DebugSystem::getInstance().showWireframe();
        for (const auto& prop : tile.props) {
            propManager.drawProp3D(prop, x, y, wireframe);
        }
        drawTileResources3D(x, y, tile.resources);
    }
}

void GameWorld::drawTileResources3D(int x, int y, const TileResources& resources) {
    if (!_resourceModelsLoaded) return;

    Vector3 basePos = { (float)x, 1.0f, (float)y };
    int resourceIndex = 0;

    if (resources.food > 0) {
        Vector3 turkeyPos = getResourcePosition(resourceIndex++, basePos);
        DrawModel(_turkeyModel, turkeyPos, 0.2f, WHITE);
    }

    if (resources.eggs > 0) {
        Vector3 eggPos = getResourcePosition(resourceIndex++, basePos);
        DrawModel(_eggModel, eggPos, 0.002f, WHITE);
    }

    if (resources.linemate > 0) {
        Vector3 linematePos = getResourcePosition(resourceIndex++, basePos);
        linematePos.y += 0.05f;
        DrawModel(_linemateModel, linematePos, 0.35f, WHITE);
    }

    if (resources.deraumere > 0) {
        Vector3 deraumerePos = getResourcePosition(resourceIndex++, basePos);
        deraumerePos.y += 0.025f;
        DrawModelEx(_deraumereModel, deraumerePos, {1, 0, 0}, 90.0f, {0.015f, 0.015f, 0.015f}, WHITE);
    }

    if (resources.sibur > 0) {
        Vector3 siburPos = getResourcePosition(resourceIndex++, basePos);
        DrawModel(_siburModel, siburPos, 0.02f, WHITE);
    }

    if (resources.mendiane > 0) {
        Vector3 mendianePos = getResourcePosition(resourceIndex++, basePos);
        DrawModel(_mendianeModel, mendianePos, 0.07f, WHITE);
    }

    if (resources.phiras > 0) {
        Vector3 phirasPos = getResourcePosition(resourceIndex++, basePos);
        phirasPos.y += 0.05f;
        DrawModel(_phirasModel, phirasPos, 0.2f, WHITE);
    }

    if (resources.thystame > 0) {
        Vector3 thystamePos = getResourcePosition(resourceIndex++, basePos);
        thystamePos.y += 0.025f;
        DrawModelEx(_thystameModel, thystamePos, {1, 0, 0}, 90.0f, {0.02f, 0.02f, 0.02f}, WHITE);
    }
}

void GameWorld::loadResources() {
    std::vector<std::pair<std::string, TileType>> modelsToLoad = {
        {"assets/environment/ground_grass.obj", TileType::GROUND_GRASS},
        {"assets/environment/cliff_rock.obj", TileType::CLIFF_ROCK},
        {"assets/environment/cliff_corner_rock.obj", TileType::CLIFF_CORNER_ROCK}
    };

    for (const auto& [path, type] : modelsToLoad) {
        if (FileExists(path.c_str())) {
            Model model = LoadModel(path.c_str());
            if (model.meshCount > 0) {
                ModelResource resource;
                resource.model = model;
                resource.name = path;
                resource.type = type;
                _models.push_back(resource);
            }
        }
    }
}

void GameWorld::loadResourceModels() {
    const std::vector<std::pair<std::string, Model*>> resourceModels = {
        {"assets/resources/turkey.glb", &_turkeyModel},
        {"assets/resources/egg.glb", &_eggModel},
        {"assets/resources/linemate.glb", &_linemateModel},
        {"assets/resources/deraumere.glb", &_deraumereModel},
        {"assets/resources/sibur.glb", &_siburModel},
        {"assets/resources/mendiane.glb", &_mendianeModel},
        {"assets/resources/phiras.glb", &_phirasModel},
        {"assets/resources/thystame.glb", &_thystameModel}
    };

    bool allLoaded = true;
    for (const auto& [path, modelPtr] : resourceModels) {
        if (FileExists(path.c_str())) {
            *modelPtr = LoadModel(path.c_str());
            if (modelPtr->meshCount == 0) {
                allLoaded = false;
                std::cerr << "Warning: Failed to load model " << path << std::endl;
            }
        } else {
            allLoaded = false;
            std::cerr << "Warning: Model file not found: " << path << std::endl;
        }
    }

    _resourceModelsLoaded = allLoaded;
    if (_resourceModelsLoaded) {
        std::cout << "INFO: All resource models loaded successfully" << std::endl;
    }
}

void GameWorld::unloadResources() {
    try {
        for (auto& resource : _models) {
            if (IsWindowReady() && resource.model.meshCount > 0) {
                UnloadModel(resource.model);
            }
        }
        _models.clear();
        
        if (_resourceModelsLoaded && IsWindowReady()) {
            const std::vector<Model*> resourceModels = {
                &_turkeyModel, &_eggModel, &_linemateModel, &_deraumereModel,
                &_siburModel, &_mendianeModel, &_phirasModel, &_thystameModel
            };
            
            for (Model* model : resourceModels) {
                if (model->meshCount > 0) {
                    UnloadModel(*model);
                }
            }
        }
        
        PropManager::getInstance().cleanup();
    } catch (...) {
        std::cout << "ERROR: Exception in unloadResources" << std::endl;
    }
}

Model* GameWorld::getModelForTileType(TileType type) {
    for (auto& resource : _models) {
        if (resource.type == type) {
            return &resource.model;
        }
    }
    return nullptr;
}

Color GameWorld::getColorForTileType(TileType type) {
    switch (type) {
        case TileType::GROUND_GRASS:
            return Color{34, 139, 34, 255};
        case TileType::CLIFF_ROCK:
            return Color{105, 105, 105, 255};
        case TileType::CLIFF_CORNER_ROCK:
            return Color{69, 69, 69, 255};
        default:
            return MAGENTA;
    }
}

bool GameWorld::isCorner(int x, int y) const {
    return (x == 0 && y == 0) ||
           (x == _totalWidth - 1 && y == 0) ||
           (x == 0 && y == _totalHeight - 1) ||
           (x == _totalWidth - 1 && y == _totalHeight - 1);
}

bool GameWorld::isEdge(int x, int y) const {
    return x == 0 || x == _totalWidth - 1 || 
           y == 0 || y == _totalHeight - 1;
}

bool GameWorld::isTopLeft(int x, int y) const {
    return x == 0 && y == 0;
}

bool GameWorld::isTopRight(int x, int y) const {
    return x == _totalWidth - 1 && y == 0;
}

bool GameWorld::isBottomLeft(int x, int y) const {
    return x == 0 && y == _totalHeight - 1;
}

bool GameWorld::isBottomRight(int x, int y) const {
    return x == _totalWidth - 1 && y == _totalHeight - 1;
}

bool GameWorld::isTopEdge(int x, int y) const {
    return y == 0 && x > 0 && x < _totalWidth - 1;
}

bool GameWorld::isBottomEdge(int x, int y) const {
    return y == _totalHeight - 1 && x > 0 && x < _totalWidth - 1;
}

bool GameWorld::isLeftEdge(int x, int y) const {
    return x == 0 && y > 0 && y < _totalHeight - 1;
}

bool GameWorld::isRightEdge(int x, int y) const {
    return x == _totalWidth - 1 && y > 0 && y < _totalHeight - 1;
}

float GameWorld::getTileRotation(int x, int y, TileType type) const {
    if (type == TileType::CLIFF_CORNER_ROCK) {
        if (isTopLeft(x, y)) return -90.0f;
        if (isTopRight(x, y)) return 180.0f;
        if (isBottomLeft(x, y)) return 0.0f;
        if (isBottomRight(x, y)) return 90.0f;
    } else if (type == TileType::CLIFF_ROCK) {
        if (isTopEdge(x, y)) return 180.0f;
        if (isBottomEdge(x, y)) return 0.0f;
        if (isLeftEdge(x, y)) return -90.0f;
        if (isRightEdge(x, y)) return 90.0f;
    }
    return 0.0f;
}

GameWorld::Tile GameWorld::getTileAt(int x, int y) const {
    if (x < 0 || x >= _totalWidth || y < 0 || y >= _totalHeight) {
        return Tile(TileType::GROUND_GRASS, 0.0f);
    }
    return _tileMap[y][x];
}

Vector3 GameWorld::getTilePosition3D(int x, int y) const {
    return Vector3{ (float)x, 0.0f, (float)y };
}
