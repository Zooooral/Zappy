/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** PropManager
*/

#ifndef PROPMANAGER_HPP_
    #define PROPMANAGER_HPP_

    #include <vector>
    #include <string>
    #include <random>
    #include <cstring>

    #include "raylib.h"

struct Prop {
    std::string name;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    Model* model;

    Prop() : position({0, 0, 0}), rotation({0, 0, 0}), scale({1, 1, 1}), model(nullptr) {}
    Prop(const std::string& n, const Vector3& pos, const Vector3& rot, const Vector3& sc, Model* m)
        : name(n), position(pos), rotation(rot), scale(sc), model(m) {}
};

struct PropModelResource {
    Model model;
    std::string name;
    std::string filename;
    bool unloaded = false;

    ~PropModelResource() {
        if (!unloaded && IsWindowReady() && model.meshCount > 0) {
            UnloadModel(model);
            unloaded = true;
        }
    }

    PropModelResource(const PropModelResource& other)
        : name(other.name), filename(other.filename), unloaded(other.unloaded) {
        model = LoadModel(other.filename.c_str());
    }

    PropModelResource& operator=(const PropModelResource& other) {
        if (this != &other) {
            if (!unloaded && IsWindowReady() && model.meshCount > 0) {
                UnloadModel(model);
            }
            name = other.name;
            filename = other.filename;
            unloaded = other.unloaded;
            model = LoadModel(other.filename.c_str());
        }
        return *this;
    }

    PropModelResource() : unloaded(false) {
        memset(&model, 0, sizeof(Model));
    }
};

class PropManager {
public:
    static PropManager& getInstance();

    void initialize();
    void cleanup();

    void generatePropsForTile(std::vector<Prop>& props);
    void drawProp3D(const Prop& prop, int tileX, int tileY, bool wireframe = false);
    void drawProp2D(const Prop& prop, Vector2 tileCenter, float tileSize);

    const std::vector<PropModelResource>& getPropModels() const { return _propModels; }

private:
    PropManager();
    ~PropManager();
    PropManager(const PropManager&) = delete;
    PropManager& operator=(const PropManager&) = delete;

    std::vector<PropModelResource> _propModels;
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _propCountDist;
    std::uniform_real_distribution<float> _positionDist;
    std::uniform_real_distribution<float> _rotationDist;
    std::uniform_real_distribution<float> _scaleDist;
    bool _cleaned_up;

    void loadPropModels();
    bool isMushroomOrFlower(const std::string& name) const;
    bool isRock(const std::string& name) const;
    bool isPropColliding(const Vector3& newPos, const std::vector<Prop>& existingProps, float minDistance = 0.3f) const;
    Model* getPropModel(const std::string& name);
};

#endif
