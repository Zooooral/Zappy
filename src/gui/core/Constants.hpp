/*
** EPITECH PROJECT, 2025
** src/gui/core/Constants.hpp
** File description:
** Application constants
*/

#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

namespace zappy {
namespace constants {

// Screen dimensions
constexpr int DEFAULT_SCREEN_WIDTH = 1920;
constexpr int DEFAULT_SCREEN_HEIGHT = 1080;
constexpr int TARGET_FPS = 60;

// UI spacing and sizing
constexpr float UI_SPACING_SMALL = 0.05f;
constexpr float UI_BUTTON_HEIGHT = 40.0f;
constexpr float UI_BUTTON_WIDTH = 150.0f;

// Animation timing
constexpr float FADE_TIME = 1.0f;
constexpr float SHOW_TIME = 1.2f;
constexpr float PARTICLE_LIFETIME_MIN = 0.8f;
constexpr float PARTICLE_LIFETIME_MAX = 1.5f;

// Network settings
constexpr int DEFAULT_PORT = 4242;
constexpr const char* DEFAULT_HOST = "localhost";
constexpr int NETWORK_BUFFER_SIZE = 1024;
constexpr int NETWORK_TIMEOUT_US = 100000;

// Audio settings
constexpr float DEFAULT_VOLUME = 0.7f;
constexpr float BIRDS_VOLUME_MULTIPLIER = 0.3f;
constexpr float WAVES_VOLUME_MULTIPLIER = 0.5f;

// Character rendering
constexpr float CHARACTER_SCALE = 0.000025f;
constexpr float CHARACTER_Y_OFFSET = -0.16f;
constexpr float LEVEL_TEXT_Y_OFFSET = 1.2f;
constexpr float SPARKLE_RADIUS_BASE = 18.0f;
constexpr float SPARKLE_RADIUS_VARIATION = 4.0f;

// Fonts
constexpr int FONT_SIZE_SMALL = 16;
constexpr int FONT_SIZE_MEDIUM = 24;
constexpr int FONT_SIZE_LARGE = 32;
constexpr int FONT_SIZE_TITLE = 44;

}
}

#endif /* !CONSTANTS_HPP_ */
