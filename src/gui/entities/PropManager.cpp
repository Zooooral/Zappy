/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** PropManager
*/

#include "PropManager.hpp"
#include "../core/DebugSystem.hpp"
#include <iostream>
#include <algorithm>

PropManager& PropManager::getInstance() {
    static PropManager instance;
    return instance;
}

PropManager::PropManager() : _rng(std::random_device{}()), _propCountDist(1, 2), 
                             _positionDist(-0.4f, 0.4f), _rotationDist(0.0f, 360.0f), 
                             _scaleDist(0.8f, 1.2f), _cleaned_up(false) {
}

PropManager::~PropManager() {
    cleanup();
}

void PropManager::initialize() {
    if (!_cleaned_up) {
        loadPropModels();
    }
}

void PropManager::cleanup() {
    if (_cleaned_up) return;

    _cleaned_up = true;

    try {
        for (auto& resource : _propModels) {
            if (IsWindowReady() && resource.model.meshCount > 0) {
                UnloadModel(resource.model);
                resource.unloaded = true;
            }
        }
        _propModels.clear();
        std::cout << "INFO: PropManager cleanup completed" << std::endl;
    } catch (...) {
        std::cout << "ERROR: Exception in PropManager::cleanup" << std::endl;
    }
}

void PropManager::generatePropsForTile(std::vector<Prop>& props) {
    if (_propModels.empty() || _cleaned_up) return;

    int propCount = _propCountDist(_rng);
    props.clear();

    std::vector<std::string> usedProps;

    for (int i = 0; i < propCount; i++) {
        std::vector<PropModelResource*> availableProps;
        for (auto& propModel : _propModels) {
            if (std::find(usedProps.begin(), usedProps.end(), propModel.name) == usedProps.end()) {
                availableProps.push_back(&propModel);
            }
        }

        if (availableProps.empty()) break;

        std::uniform_int_distribution<size_t> propDist(0, availableProps.size() - 1);
        PropModelResource* selectedProp = availableProps[propDist(_rng)];

        Vector3 propPosition;
        bool validPosition = false;
        int attempts = 0;
        const int maxAttempts = 20;

        while (!validPosition && attempts < maxAttempts) {
            propPosition = {
                _positionDist(_rng),
                1.0f,
                _positionDist(_rng)
            };

            validPosition = !isPropColliding(propPosition, props);
            attempts++;
        }

        if (validPosition) {
            Vector3 rotation = {0.0f, _rotationDist(_rng), 0.0f};
            Vector3 scale;

            if (isMushroomOrFlower(selectedProp->name)) {
                float scaleFactor = _scaleDist(_rng);
                scale = {scaleFactor, scaleFactor, scaleFactor};
            } else if (isRock(selectedProp->name)) {
                scale = {0.25f, 0.25f, 0.25f};
            } else {
                scale = {0.55f, 0.55f, 0.55f};
            }

            props.emplace_back(selectedProp->name, propPosition, rotation, scale, &selectedProp->model);
            usedProps.push_back(selectedProp->name);
        }
    }
}

void PropManager::drawProp3D(const Prop& prop, int tileX, int tileY, bool wireframe) {
    if (!prop.model || prop.model->meshCount == 0 || _cleaned_up) return;
    if (!DebugSystem::getInstance().showObstacles()) return;

    Vector3 tileCenter = {(float)tileX, 0.0f, (float)tileY};
    Vector3 worldPos = {
        tileCenter.x + prop.position.x,
        tileCenter.y + prop.position.y,
        tileCenter.z + prop.position.z
    };

    Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };

    if (wireframe) {
        DrawModelWires(*prop.model, worldPos, prop.scale.x, WHITE);
    } else {
        DrawModelEx(*prop.model, worldPos, rotationAxis, prop.rotation.y, prop.scale, WHITE);
    }

    if (DebugSystem::getInstance().showDebugSpheres()) {
        DrawSphere(Vector3{worldPos.x, worldPos.y + 0.1f, worldPos.z}, 0.02f, BLUE);
    }
}

void PropManager::drawProp2D(const Prop& prop, Vector2 tileCenter, float tileSize) {
    (void)prop;
    (void)tileCenter;
    (void)tileSize;
}

void PropManager::loadPropModels() {
    if (_cleaned_up) return;

    std::vector<std::string> propFiles = {
        "tree_oak_dark.obj",
        "tree_oak_fall.obj",
        "tree_pineDefaultA.obj",
        "tree_pineSmallA.obj",
        "tree_tall_dark.obj",
        "stump_oldTall.obj",
        "stump_roundDetailed.obj",
        "flower_purpleA.obj",
        "flower_redA.obj",
        "plant_bushDetailed.obj",
        "plant_bushLarge.obj",
        "rock_largeA.obj",
        "rock_smallFlatA.obj",
        "rock_tallA.obj",
        "stone_largeA.obj",
        "stone_smallA.obj",
        "stone_tallA.obj",
        "mushroom_red.obj",
        "mushroom_tanGroup.obj",
        "grass_leafs.obj",
        "log_stack.obj"
    };

    for (const std::string& filename : propFiles) {
        std::string path = "assets/environment/" + filename;
        if (FileExists(path.c_str())) {
            Model model = LoadModel(path.c_str());
            if (model.meshCount > 0) {
                PropModelResource resource;
                resource.model = model;
                resource.filename = path;
                resource.name = filename.substr(0, filename.find_last_of('.'));
                resource.unloaded = false;
                _propModels.push_back(resource);
            }
        }
    }

    std::cout << "INFO: Loaded " << _propModels.size() << " prop models" << std::endl;
}

bool PropManager::isMushroomOrFlower(const std::string& name) const {
    return name.find("mushroom") != std::string::npos || 
           name.find("flower") != std::string::npos;
}

bool PropManager::isRock(const std::string& name) const {
    return name.find("rock") != std::string::npos;
}

bool PropManager::isPropColliding(const Vector3& newPos, const std::vector<Prop>& existingProps, float minDistance) const {
    for (const auto& prop : existingProps) {
        float dx = newPos.x - prop.position.x;
        float dz = newPos.z - prop.position.z;
        float distance = sqrt(dx * dx + dz * dz);
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

Model* PropManager::getPropModel(const std::string& name) {
    if (_cleaned_up) return nullptr;

    for (auto& resource : _propModels) {
        if (resource.name == name) {
            return &resource.model;
        }
    }
    return nullptr;
}
