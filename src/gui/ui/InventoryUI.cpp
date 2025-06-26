/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** InventoryUI
*/

#include "InventoryUI.hpp"
#include "../core/FontManager.hpp"
#include <algorithm>

InventoryUI::InventoryUI(const Vector2& position, const Vector2& size)
    : AComponent(position, size) {
}

void InventoryUI::update(float dt) {
    (void)dt;
    
    if (_visible && IsKeyPressed(KEY_ESCAPE)) {
        _visible = false;
    }
    
    if (_visible && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!isHovered()) {
            _visible = false;
        }
    }
}

void InventoryUI::draw() const {
    if (!_visible || !_character) return;
    
    Rectangle bounds = getBounds();
    DrawRectangleRounded(bounds, 0.1f, 5, Fade(BLACK, 0.7f));
    DrawRectangleRoundedLines(bounds, 0.1f, 5, BLACK);
    
    drawCharacterInfo();
    drawInventoryGrid();
    
    Font font = FontManager::getInstance().getFont("medium");
    const char* closeText = "Press ESC or click outside to close";
    Vector2 textSize = MeasureTextEx(font, closeText, 14, 1);
    DrawTextEx(font, closeText, 
               {_position.x + (_size.x - textSize.x) / 2, _position.y + _size.y - 25}, 
               14, 1, LIGHTGRAY);
}

void InventoryUI::setCharacter(Character* character) {
    _character = character;
    _visible = (character != nullptr);
}

std::vector<InventoryUI::InventoryItem> InventoryUI::getInventoryItems() const {
    if (!_character) return {};
    
    const CharacterInventory& inv = _character->getInventory();
    return {
        {"Food", inv.food, YELLOW},
        {"Linemate", inv.linemate, getResourceColor("linemate")},
        {"Deraumere", inv.deraumere, getResourceColor("deraumere")},
        {"Sibur", inv.sibur, getResourceColor("sibur")},
        {"Mendiane", inv.mendiane, getResourceColor("mendiane")},
        {"Phiras", inv.phiras, getResourceColor("phiras")},
        {"Thystame", inv.thystame, getResourceColor("thystame")}
    };
}

void InventoryUI::drawInventoryGrid() const {
    if (!_character) return;
    
    Font font = FontManager::getInstance().getFont("medium");
    auto items = getInventoryItems();
    
    float startY = _position.y + 80;
    float itemHeight = 30;
    float padding = 10;
    
    for (size_t i = 0; i < items.size(); ++i) {
        float y = startY + i * (itemHeight + padding);
        
        Rectangle itemRect = {_position.x + 20, y, _size.x - 40, itemHeight};
        DrawRectangleRounded(itemRect, 0.3f, 10, Fade(items[i].color, 0.2f));
        DrawRectangleRoundedLines(itemRect, 0.3f, 10, items[i].color);
        
        DrawTextEx(font, items[i].name.c_str(), 
                   {itemRect.x + 10, itemRect.y + 5}, 18, 1, WHITE);
        
        std::string quantityText = std::to_string(items[i].quantity);
        Vector2 quantitySize = MeasureTextEx(font, quantityText.c_str(), 18, 1);
        DrawTextEx(font, quantityText.c_str(),
                   {itemRect.x + itemRect.width - quantitySize.x - 10, itemRect.y + 5},
                   18, 1, items[i].color);
    }
}

void InventoryUI::drawCharacterInfo() const {
    if (!_character) return;
    
    Font font = FontManager::getInstance().getFont("medium");
    
    std::string title = "Character #" + std::to_string(_character->getId());
    Vector2 titleSize = MeasureTextEx(font, title.c_str(), 24, 1);
    DrawTextEx(font, title.c_str(),
               {_position.x + (_size.x - titleSize.x) / 2, _position.y + 15},
               24, 1, WHITE);
    
    std::string teamText = "Team: " + _character->getTeam();
    DrawTextEx(font, teamText.c_str(), {_position.x + 20, _position.y + 45}, 18, 1, BLUE);
    
    std::string levelText = "Level: " + std::to_string(_character->getLevel());
    Vector2 levelSize = MeasureTextEx(font, levelText.c_str(), 18, 1);
    DrawTextEx(font, levelText.c_str(),
               {_position.x + _size.x - levelSize.x - 20, _position.y + 45},
               18, 1, LIME);
}

Color InventoryUI::getResourceColor(const std::string& resourceName) const {
    if (resourceName == "linemate") return ORANGE;
    if (resourceName == "deraumere") return PURPLE;
    if (resourceName == "sibur") return SKYBLUE;
    if (resourceName == "mendiane") return PINK;
    if (resourceName == "phiras") return LIME;
    if (resourceName == "thystame") return MAROON;
    return WHITE;
}
