/*
** EPITECH PROJECT, 2025
** src/gui/core/FontManager.cpp
** File description:
** FontManager
*/

#include "FontManager.hpp"

#include <iostream>

FontManager &FontManager::getInstance()
{
    static FontManager instance;
    return instance;
}

FontManager::FontManager()
{
    initialize();
}

FontManager::~FontManager()
{
}

void FontManager::initialize()
{
    if (_initialized)
        return;

    Font thin = LoadFontEx("assets/fonts/thin.ttf", 64, NULL, 256);
    SetTextureFilter(thin.texture, TEXTURE_FILTER_BILINEAR);
    _fonts["thin"] = thin;

    Font medium = LoadFontEx("assets/fonts/medium.ttf", 64, NULL, 256);
    SetTextureFilter(medium.texture, TEXTURE_FILTER_BILINEAR);
    _fonts["medium"] = medium;

    Font bold = LoadFontEx("assets/fonts/bold.ttf", 64, NULL, 256);
    SetTextureFilter(bold.texture, TEXTURE_FILTER_BILINEAR);
    _fonts["bold"] = bold;

    _initialized = true;
}

Font FontManager::getFont(const std::string &type)
{
    if (!_initialized) initialize();

    auto it = _fonts.find(type);
    if (it != _fonts.end()) {
        return it->second;
    }
    std::cerr << "Warning: Font type '" << type << "' not found, returning default" << std::endl;
    return _fonts["medium"];
}

void FontManager::unloadFonts()
{
    if (!_initialized)
        return;

    for (auto &pair : _fonts) {
        UnloadFont(pair.second);
    }
    _fonts.clear();
    _initialized = false;
}
