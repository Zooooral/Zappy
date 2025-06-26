/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel [WSL: Ubuntu]
** File description:
** EggManager
*/

#include "EggManager.hpp"
#include "../core/GameWorld.hpp"

EggManager& EggManager::getInstance() {
    static EggManager instance;
    return instance;
}

void EggManager::addEgg(int eggId, int x, int y, int playerId) {
    _eggs[eggId] = std::make_unique<Egg>(eggId, x, y, playerId);
    updateTileEggs(x, y);
}

void EggManager::removeEgg(int eggId) {
    auto it = _eggs.find(eggId);
    if (it != _eggs.end()) {
        int x = it->second->x;
        int y = it->second->y;
        _eggs.erase(it);
        updateTileEggs(x, y);
    }
}

void EggManager::setEggHatching(int eggId, bool hatching) {
    auto it = _eggs.find(eggId);
    if (it != _eggs.end()) {
        it->second->isHatching = hatching;
    }
}

bool EggManager::hasEggAt(int x, int y) const {
    for (const auto& [id, egg] : _eggs) {
        if (egg->x == x && egg->y == y) {
            return true;
        }
    }
    return false;
}

int EggManager::getEggCountAt(int x, int y) const {
    int count = 0;
    for (const auto& [id, egg] : _eggs) {
        if (egg->x == x && egg->y == y) {
            count++;
        }
    }
    return count;
}

void EggManager::updateTileEggs(int x, int y) {
    GameWorld::TileResources resources = GameWorld::getInstance().getTileAt(x, y).resources;
    resources.eggs = getEggCountAt(x, y);
    GameWorld::getInstance().updateTileResources(x, y, resources);
}

void EggManager::cleanup() {
    _eggs.clear();
}
