/*
** EPITECH PROJECT, 2025
** src/gui/core/FontManager.hpp
** File description:
** FontManager
*/

#ifndef FONTMANAGER_HPP_
    #define FONTMANAGER_HPP_

#include "raylib.h"

#include <string>
#include <unordered_map>

class FontManager {
public:
    static FontManager &getInstance();
    Font getFont(const std::string &type);
    void unloadFonts();

private:
    FontManager();
    ~FontManager();
    FontManager(const FontManager &) = delete;
    FontManager &operator=(const FontManager &) = delete;

    std::unordered_map<std::string, Font> _fonts;
    bool _initialized = false;
    void initialize();
};

#endif /* !FONTMANAGER_HPP_ */
