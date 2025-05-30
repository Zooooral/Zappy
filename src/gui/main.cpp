/*
** EPITECH PROJECT, 2025
** Zappy
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
#include "screens/SplashScreen.hpp"
#include "screens/MainMenu.hpp"
#include "screens/GameScreen.hpp"
#include "screens/SettingsMenu.hpp"
#include "screens/EndScreen.hpp"
#include "screens/ConnectingScreen.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>

static bool g_cleanup_done = false;

void cleanup_resources()
{
    if (g_cleanup_done) return;
    g_cleanup_done = true;
    
    try {
        SoundManager::getInstance().stopMusic();
        SoundManager::getInstance().stopAllSounds();
        FontManager::getInstance().unloadFonts();
        if (IsWindowReady()) {
            CloseWindow();
        }
    } catch (...) {
    }
}

void signalHandler(int signum) {
    std::cout << "ERROR: Received signal " << signum << std::endl;
    cleanup_resources();
    std::exit(signum);
}

void printHelp(const char* programName)
{
    std::cout << "USAGE: " << programName << " -p port -h machine" << std::endl;
    std::cout << "  option\tdescription" << std::endl;
    std::cout << "  -p port\tport number" << std::endl;
    std::cout << "  -h machine\thostname of the server" << std::endl;
}

bool parseArgs(int argc, char** argv)
{
    ConfigManager& config = ConfigManager::getInstance();

    if (argc == 1) {
        return true;
    }

    if (argc == 2 && (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0)) {
        printHelp(argv[0]);
        return false;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            config.setPort(std::atoi(argv[i + 1]));
            i++;
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            config.setHost(argv[i + 1]);
            i++;
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            printHelp(argv[0]);
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    
    if (!parseArgs(argc, argv)) {
        return 0;
    }

    const int screenWidth = 1920;
    const int screenHeight = 1080;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Zappy - A Tribute to Zaphod Beeblebrox");
    SetTargetFPS(60);

    bool shouldQuit = false;
    
    try {
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
        std::cout << "ERROR: Exception in main: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "ERROR: Unknown exception in main" << std::endl;
    }

    cleanup_resources();
    
    std::cout << "INFO: Application ended successfully" << std::endl;
    return 0;
}
// Updated: 2025-05-31 00:53:39
// Updated: 2025-05-31 00:53:54
