/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel
** File description:
** main
*/

#include "raylib.h"
#include "core/ConfigManager.hpp"
#include "core/FontManager.hpp"
#include "core/SoundManager.hpp"
#include "core/GameStateManager.hpp"
#include "core/GameWorld.hpp"
#include "core/Environment.hpp"
#include "core/Constants.hpp"
#include "core/ArgumentValidator.hpp"
#include "ui/KeyBindButton.hpp"
#include "screens/SplashScreen.hpp"
#include "screens/MainMenu.hpp"
#include "screens/GameScreen.hpp"
#include "screens/SettingsMenu.hpp"
#include "screens/EndScreen.hpp"
#include "screens/ConnectingScreen.hpp"
#include "network/NetworkPlatform.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <atomic>

void cleanup_resources() noexcept
{
    try {
        SoundManager::getInstance().stopMusic();
        SoundManager::getInstance().stopAllSounds();
        FontManager::getInstance().unloadFonts();
        zappy::network::NetworkPlatform::cleanup();
        if (IsWindowReady()) {
            CloseWindow();
        }
    } catch (...) {
    }
}

int main(int argc, char** argv)
{
    ArgumentValidator::Arguments args = ArgumentValidator::parseArguments(argc, argv);
    
    if (!args.valid) {
        std::cerr << "Error: " << args.errorMessage << std::endl;
        ArgumentValidator::printHelp(argv[0]);
        return 84;
    }

    if (!zappy::network::NetworkPlatform::initialize()) {
        std::cerr << "Error: Failed to initialize network platform" << std::endl;
        return 84;
    }

    if (!ArgumentValidator::validateConnection(args.host, args.port)) {
        std::cerr << "Error: Cannot connect to server at " << args.host << ":" << args.port << std::endl;
        std::cerr << "Please ensure the server is running and accessible." << std::endl;
        zappy::network::NetworkPlatform::cleanup();
        return 84;
    }

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(zappy::constants::DEFAULT_SCREEN_WIDTH, 
               zappy::constants::DEFAULT_SCREEN_HEIGHT, 
               "Zappy - A Tribute to Zaphod Beeblebrox");
    SetTargetFPS(zappy::constants::TARGET_FPS);

    bool shouldQuit = false;
    
    try {
        ConfigManager& config = ConfigManager::getInstance();
        config.setHost(args.host);
        config.setPort(args.port);

        GameStateManager& stateManager = GameStateManager::getInstance();
        
        stateManager.registerState("splash", std::make_unique<SplashScreen>());
        stateManager.registerState("main_menu", std::make_unique<MainMenu>());
        stateManager.registerState("connecting", std::make_unique<ConnectingScreen>());
        stateManager.registerState("game", std::make_unique<GameScreen>());
        stateManager.registerState("settings", std::make_unique<SettingsMenu>());
        stateManager.registerState("end_screen", std::make_unique<EndScreen>());

        while (!WindowShouldClose() && !shouldQuit && !stateManager.shouldQuit()) {
            float dt = GetFrameTime();
            
            stateManager.update(dt);
            
            BeginDrawing();
            stateManager.draw();
            EndDrawing();
            
            if (stateManager.getCurrentState() == "main_menu") {
                MainMenu* mainMenu = static_cast<MainMenu*>(stateManager.getStateInstance("main_menu"));
                if (mainMenu && mainMenu->isFinished()) {
                    shouldQuit = true;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception in main: " << e.what() << std::endl;
        cleanup_resources();
        return 84;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception in main" << std::endl;
        cleanup_resources();
        return 84;
    }

    cleanup_resources();
    
    std::cout << "INFO: Application ended successfully" << std::endl;
    return 0;
}
