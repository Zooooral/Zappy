/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Environment
*/

#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include "raylib.h"
#include <vector>

class Environment {
public:
    static Environment& getInstance();
    void initialize();
    void update(float dt);
    void drawSky(Camera camera);
    void drawSea(Camera camera);
    void cleanup();

private:
    Environment() = default;
    ~Environment() = default;
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;

    Model _seaModel;
    Shader _waterShader;
    std::vector<float> _originalVertices;
    
    float _time = 0.0f;
    bool _initialized = false;

    void createSeaMesh();
    void createWaterShader();
    void updateSeaWaves();
};

#endif
