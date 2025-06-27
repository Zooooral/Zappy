/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel [WSL: Ubuntu]
** File description:
** EggManager
*/

#ifndef EGGMANAGER_HPP_
#define EGGMANAGER_HPP_

#include <unordered_map>
#include <memory>

struct Egg {
    int id;
    int x;
    int y;
    int playerId;
    bool isHatching;
    
    Egg(int eggId, int posX, int posY, int pId) 
        : id(eggId), x(posX), y(posY), playerId(pId), isHatching(false) {}
};

class EggManager {
public:
    static EggManager& getInstance();
    
    void addEgg(int eggId, int x, int y, int playerId);
    void removeEgg(int eggId);
    void setEggHatching(int eggId, bool hatching);
    bool hasEggAt(int x, int y) const;
    int getEggCountAt(int x, int y) const;
    
    void cleanup();

private:
    EggManager() = default;
    ~EggManager() = default;
    EggManager(const EggManager&) = delete;
    EggManager& operator=(const EggManager&) = delete;
    
    std::unordered_map<int, std::unique_ptr<Egg>> _eggs;
    
    void updateTileEggs(int x, int y);
};

#endif
