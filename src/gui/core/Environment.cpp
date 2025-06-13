/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Environment
*/

#include <cmath>
#include <iostream>

#include "Environment.hpp"

Environment& Environment::getInstance() {
    static Environment instance;
    return instance;
}

void Environment::initialize() {
    if (_initialized) return;
    createSeaMesh();
    createWaterShader();
    _initialized = true;
}

void Environment::update(float dt) {
    _time += dt;
    if (_initialized && _seaModel.meshCount > 0) {
        updateSeaWaves();
    }
}

void Environment::drawSky(Camera camera) {
    (void)camera;
    if (!_initialized) {
        ClearBackground(Color{135, 206, 235, 255});
        return;
    }
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), 
                          Color{100, 149, 237, 255},
                          Color{176, 224, 230, 255});
}

void Environment::drawSea(Camera camera) {
    (void)camera;
    if (!_initialized || _seaModel.meshCount == 0) return;
    
    Vector3 seaPos = {0.0f, 0.0f, 0.0f};
    
    if (_waterShader.id > 0 && _seaModel.materialCount > 0) {
        float time = _time;
        SetShaderValue(_waterShader, GetShaderLocation(_waterShader, "time"), &time, SHADER_UNIFORM_FLOAT);
        
        Vector2 resolution = {(float)GetScreenWidth(), (float)GetScreenHeight()};
        SetShaderValue(_waterShader, GetShaderLocation(_waterShader, "resolution"), &resolution, SHADER_UNIFORM_VEC2);
        
        _seaModel.materials[0].shader = _waterShader;
        
        DrawModel(_seaModel, seaPos, 1.0f, WHITE);
    } else {
        DrawModel(_seaModel, seaPos, 1.0f, DARKBLUE);
    }
}

void Environment::cleanup() {
    if (_initialized) {
        if (_seaModel.meshCount > 0) UnloadModel(_seaModel);
        if (_waterShader.id > 0) UnloadShader(_waterShader);
        _initialized = false;
    }
}

void Environment::createSeaMesh() {
    const int seaWidth = 100;
    const int seaDepth = 100;
    Mesh seaMesh = GenMeshPlane(200.0f, 200.0f, seaWidth, seaDepth);
    
    if (!seaMesh.texcoords) {
        seaMesh.texcoords = (float*)malloc(seaMesh.vertexCount * 2 * sizeof(float));
        for (int i = 0; i < seaMesh.vertexCount; i++) {
            float x = seaMesh.vertices[i * 3];
            float z = seaMesh.vertices[i * 3 + 2];
            seaMesh.texcoords[i * 2] = (x + 100.0f) / 200.0f;
            seaMesh.texcoords[i * 2 + 1] = (z + 100.0f) / 200.0f;
        }
    }
    
    _seaModel = LoadModelFromMesh(seaMesh);
    
    _originalVertices.clear();
    if (seaMesh.vertexCount > 0 && seaMesh.vertices) {
        for (int i = 0; i < seaMesh.vertexCount * 3; i++) {
            _originalVertices.push_back(seaMesh.vertices[i]);
        }
    }
}

void Environment::createWaterShader() {
    const char* shaderPath = "assets/shaders/water_turbulence.glsl";
    
    if (FileExists(shaderPath)) {
        _waterShader = LoadShader(NULL, shaderPath);
        
        if (_waterShader.id > 0) {
            std::cout << "Info: Water shader loaded successfully from " << shaderPath << std::endl;
        } else {
            std::cerr << "Warning: Failed to load water shader from " << shaderPath << std::endl;
        }
    } else {
        std::cerr << "Warning: Water shader file not found at " << shaderPath << std::endl;
    }
}

void Environment::updateSeaWaves() {
    if (_seaModel.meshCount == 0 || _originalVertices.empty()) return;
    
    Mesh* mesh = &_seaModel.meshes[0];
    if (!mesh->vertices) return;
    
    for (int i = 0; i < mesh->vertexCount; i++) {
        int idx = i * 3;
        if (idx + 2 < static_cast<int>(_originalVertices.size())) {
            float x = _originalVertices[idx];
            float z = _originalVertices[idx + 2];
            
            float wave1 = sinf(x * 0.1f + _time * 2.0f) * 0.25f;
            float wave2 = sinf(z * 0.15f + _time * 1.5f) * 0.20f;
            float wave3 = sinf((x + z) * 0.08f + _time * 1.0f) * 0.12f;
            float wave4 = sinf((x - z) * 0.12f + _time * 0.8f) * 0.06f;
            
            mesh->vertices[idx + 1] = _originalVertices[idx + 1] + wave1 + wave2 + wave3 + wave4;
        }
    }
    
    UpdateMeshBuffer(*mesh, 0, mesh->vertices, mesh->vertexCount * 3 * sizeof(float), 0);
}
