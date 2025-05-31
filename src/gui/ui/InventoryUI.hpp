/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** InventoryUI
*/

#ifndef INVENTORYUI_HPP_
    #define INVENTORYUI_HPP_

#include "AComponent.hpp"
#include "../entities/Character.hpp"
#include <string>
#include <vector>

class InventoryUI : public AComponent {
public:
    InventoryUI(const Vector2& position, const Vector2& size);
    ~InventoryUI() = default;
    
    void update(float dt) override;
    void draw() const override;
    
    void setCharacter(Character* character);
    void setVisible(bool visible) { _visible = visible; }
    bool isVisible() const { return _visible; }
    
private:
    Character* _character = nullptr;
    bool _visible = false;
    
    struct InventoryItem {
        std::string name;
        int quantity;
        Color color;
    };
    
    std::vector<InventoryItem> getInventoryItems() const;
    void drawInventoryGrid() const;
    void drawCharacterInfo() const;
    Color getResourceColor(const std::string& resourceName) const;
};

#endif
