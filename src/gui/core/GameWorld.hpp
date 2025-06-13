/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameWorld
*/

#ifndef GAMEWORLD_HPP_
    #define GAMEWORLD_HPP_

    #include <vector>
    #include <string>
    #include <unordered_map>

    #include "raylib.h"
    #include "../entities/PropManager.hpp"

class GameWorld {
public:
    enum class TileType {
        GROUND_GRASS,
        CLIFF_ROCK,
        CLIFF_CORNER_ROCK
    };

    struct TileResources {
        int food = 0;
        int linemate = 0;
        int deraumere = 0;
        int sibur = 0;
        int mendiane = 0;
        int phiras = 0;
        int thystame = 0;
        int eggs = 0;
    };

    struct Tile {
        TileType type;
        float rotation;
        std::vector<Prop> props;
        TileResources resources;

        Tile() : type(TileType::GROUND_GRASS), rotation(0.0f) {}
        Tile(TileType t, float r = 0.0f) : type(t), rotation(r) {}
    };

    static GameWorld& getInstance();

    void initialize(int width, int height);
    void update(float dt);
    void draw();
    Tile getTileAt(int x, int y) const;
    Vector3 getTilePosition3D(int x, int y) const;
    int getTotalWidth() const { return _totalWidth; }
    int getTotalHeight() const { return _totalHeight; }
    int getPlayableWidth() const { return _playableWidth; }
    int getPlayableHeight() const { return _playableHeight; }
    void updateTileResources(int x, int y, const TileResources& resources);

private:
    GameWorld();
    ~GameWorld();
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    int _playableWidth = 0;
    int _playableHeight = 0;
    int _totalWidth = 0;
    int _totalHeight = 0;
    std::vector<std::vector<Tile>> _tileMap;

    struct ModelResource {
        Model model;
        std::string name;
        TileType type;
    };

    std::vector<ModelResource> _models;
    std::unordered_map<std::string, Texture2D> _textures;

    Model _turkeyModel;
    Model _eggModel;
    Model _linemateModel;
    Model _deraumereModel;
    Model _siburModel;
    Model _mendianeModel;
    Model _phirasModel;
    Model _thystameModel;
    bool _resourceModelsLoaded = false;

    void generateMap(int playableWidth, int playableHeight);
    void generateProps();
    void draw2D();
    void draw3D();
    void drawTile2D(int x, int y, const Tile& tile);
    void drawTile3D(int x, int y, const Tile& tile);
    void drawTileResources3D(int x, int y, const TileResources& resources);
    void drawTileResources2D(int x, int y, const TileResources& resources, Vector2 center, float tileSize);
    void drawModernUI();
    void loadResources();
    void loadResourceModels();
    void unloadResources();
    Model* getModelForTileType(TileType type);
    Color getColorForTileType(TileType type);
    Vector3 getResourcePosition(int index, Vector3 basePos) const;
    bool isCorner(int x, int y) const;
    bool isEdge(int x, int y) const;
    bool isTopLeft(int x, int y) const;
    bool isTopRight(int x, int y) const;
    bool isBottomLeft(int x, int y) const;
    bool isBottomRight(int x, int y) const;
    bool isTopEdge(int x, int y) const;
    bool isBottomEdge(int x, int y) const;
    bool isLeftEdge(int x, int y) const;
    bool isRightEdge(int x, int y) const;
    float getTileRotation(int x, int y, TileType type) const;
};

#endif
