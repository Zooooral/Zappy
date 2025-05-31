/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** GameScreen
*/

#include "GameScreen.hpp"
#include "EndScreen.hpp"
#include "../core/GameWorld.hpp"
#include "../core/CameraController.hpp"
#include "../core/DebugSystem.hpp"
#include "../core/GameStateManager.hpp"
#include "../entities/CharacterManager.hpp"
#include "../core/SoundManager.hpp"
#include "../network/NetworkManager.hpp"
#include "../core/ConfigManager.hpp"
#include <iostream>
#include <sstream>

GameScreen::GameScreen() {
    _backButton = std::make_unique<Button>(
        Vector2{50, (float)GetScreenHeight() - 100},
        Vector2{150, 40},
        "Back to Menu"
    );
    _backButton->setCallback([]() {
        GameStateManager::getInstance().changeState("main_menu", GameStateManager::Transition::FADE);
    });
    
    _inventoryUI = std::make_unique<InventoryUI>(
        Vector2{(float)GetScreenWidth() - 320, 330},
        Vector2{300, 400}
    );

    _mapInitialized = false;
}

GameScreen::~GameScreen() {
    CharacterManager::getInstance().cleanup();
    NetworkManager::getInstance().disconnect();
}

void GameScreen::onEnter() {
    setTransitionAlpha(1.0f);
    _finished = false;
    _shouldReturn = false;
    _mapInitialized = false;
    
    CharacterManager::getInstance().initialize();
    
    SoundManager& soundMgr = SoundManager::getInstance();
    ConfigManager& config = ConfigManager::getInstance();
    float volume = config.getVolume();
    
    soundMgr.loadSound("birds", "assets/sounds/birds.ogg");
    soundMgr.loadSound("waves", "assets/sounds/waves.ogg");
    soundMgr.playSound("birds", true);
    soundMgr.playSound("waves", true);
    soundMgr.setSoundVolume("birds", volume * 0.3f);
    soundMgr.setSoundVolume("waves", volume * 0.5f);
    
    setupNetworkAndRequestData();
}

void GameScreen::onExit() {
    CharacterManager::getInstance().cleanup();
    SoundManager::getInstance().stopAllSounds();
    NetworkManager::getInstance().disconnect();
}

void GameScreen::setupNetworkAndRequestData() {
    NetworkManager& network = NetworkManager::getInstance();
    
    network.setCommandCallback([this](const std::string& command) {
        handleServerCommand(command);
    });
    
    requestInitialGameData();
}

void GameScreen::requestInitialGameData() {
    NetworkManager& network = NetworkManager::getInstance();
    
    network.sendCommand("msz");
    network.sendCommand("mct");
    network.sendCommand("tna");
    network.sendCommand("pnw");
}

void GameScreen::handleServerCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "msz") {
        int width, height;
        if (iss >> width >> height) {
            GameWorld::getInstance().initialize(width, height);
            _mapInitialized = true;
        }
    } else if (cmd == "bct") {
        int x, y, food, linemate, deraumere, sibur, mendiane, phiras, thystame;
        if (iss >> x >> y >> food >> linemate >> deraumere >> sibur >> mendiane >> phiras >> thystame) {
            GameWorld::TileResources resources;
            resources.food = food;
            resources.linemate = linemate;
            resources.deraumere = deraumere;
            resources.sibur = sibur;
            resources.mendiane = mendiane;
            resources.phiras = phiras;
            resources.thystame = thystame;
            GameWorld::getInstance().updateTileResources(x + 1, y + 1, resources);
        }
    } else if (cmd == "pnw") {
        std::string idStr;
        int x, y, orientation, level;
        std::string team;
        if (iss >> idStr >> x >> y >> orientation >> level >> team) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                CharacterManager::getInstance().addCharacter(id, Vector3{(float)(x + 1), 1.0f, (float)(y + 1)}, team, level);
            }
        }
    } else if (cmd == "ppo") {
        std::string idStr;
        int x, y, orientation;
        if (iss >> idStr >> x >> y >> orientation) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                Character* character = CharacterManager::getInstance().getCharacter(id);
                if (character) {
                    character->setPosition(Vector3{(float)(x + 1), 1.0f, (float)(y + 1)});
                    character->setOrientation(orientation);
                }
            }
        }
    } else if (cmd == "plv") {
        std::string idStr;
        int level;
        if (iss >> idStr >> level) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                Character* character = CharacterManager::getInstance().getCharacter(id);
                if (character) {
                    character->setLevel(level);
                }
            }
        }
    } else if (cmd == "pin") {
        std::string idStr;
        int x, y, food, linemate, deraumere, sibur, mendiane, phiras, thystame;
        if (iss >> idStr >> x >> y >> food >> linemate >> deraumere >> sibur >> mendiane >> phiras >> thystame) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                Character* character = CharacterManager::getInstance().getCharacter(id);
                if (character) {
                    CharacterInventory inventory;
                    inventory.food = food;
                    inventory.linemate = linemate;
                    inventory.deraumere = deraumere;
                    inventory.sibur = sibur;
                    inventory.mendiane = mendiane;
                    inventory.phiras = phiras;
                    inventory.thystame = thystame;
                    character->setInventory(inventory);
                }
            }
        }
    } else if (cmd == "pic") {
        int x, y, level;
        if (iss >> x >> y >> level) {
            std::string playerIds;
            std::getline(iss, playerIds);
            
            std::istringstream playerStream(playerIds);
            std::string playerId;
            while (playerStream >> playerId) {
                if (playerId.front() == '#') {
                    int id = std::stoi(playerId.substr(1));
                    Character* character = CharacterManager::getInstance().getCharacter(id);
                    if (character) {
                        character->setElevating(true);
                    }
                }
            }
        }
    } else if (cmd == "pie") {
        int x, y, result;
        if (iss >> x >> y >> result) {
            CharacterManager::getInstance().endAllElevations();
        }
    } else if (cmd == "pdi") {
        std::string idStr;
        if (iss >> idStr) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                CharacterManager::getInstance().removeCharacter(id);
            }
        }
    } else if (cmd == "seg") {
        std::string teamName;
        if (iss >> teamName) {
            triggerGameEnd(teamName);
        }
    }
}

void GameScreen::update(float dt) {
    NetworkManager::getInstance().update();
    
    SoundManager::getInstance().updateSounds();
    
    if (_mapInitialized) {
        GameWorld::getInstance().update(dt);
    }
    
    CharacterManager::getInstance().update(dt);
    _backButton->update(dt);
    _inventoryUI->update(dt);
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (_inventoryUI->isVisible()) {
            _inventoryUI->setVisible(false);
        } else {
            _shouldReturn = true;
        }
    }
    
    if (_mapInitialized) {
        CameraController& cam = CameraController::getInstance();
        if (cam.is3DMode()) {
            Character* hoveredChar = CharacterManager::getInstance().getHoveredCharacter(cam.getCamera3D());
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredChar) {
                CharacterManager::getInstance().setSelectedCharacter(hoveredChar);
                _inventoryUI->setCharacter(hoveredChar);
            }
        }
    }
    
    if (IsKeyPressed(KEY_G)) {
        triggerGameEnd("Team Alpha");
    }
}

void GameScreen::draw() {
    if (_mapInitialized) {
        GameWorld::getInstance().draw();
        
        CameraController& cam = CameraController::getInstance();
        if (cam.is3DMode()) {
            CharacterManager::getInstance().draw3D(cam.getCamera3D());
        } else {
            CharacterManager::getInstance().draw2D(cam.getCamera2D());
        }
    } else {
        ClearBackground(BLACK);
        const char* loadingText = "Loading game data...";
        int textWidth = MeasureText(loadingText, 40);
        DrawText(loadingText, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2, 40, WHITE);
    }
    
    _backButton->draw();
    _inventoryUI->draw();
    
    NetworkManager::ConnectionState connectionState = NetworkManager::getInstance().getConnectionState();
    const char* stateText = "Disconnected";
    Color stateColor = RED;
    
    switch (connectionState) {
        case NetworkManager::ConnectionState::CONNECTING:
            stateText = "Connecting...";
            stateColor = YELLOW;
            break;
        case NetworkManager::ConnectionState::CONNECTED:
            stateText = "Connected";
            stateColor = ORANGE;
            break;
        case NetworkManager::ConnectionState::AUTHENTICATED:
            stateText = "Authenticated";
            stateColor = GREEN;
            break;
        case NetworkManager::ConnectionState::ERROR:
            stateText = "Connection Error";
            stateColor = RED;
            break;
        default:
            break;
    }
    
    DrawText(stateText, 10, GetScreenHeight() - 30, 20, stateColor);
    
    if (_shouldReturn) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        const char* text = "Press ESCAPE again to return to menu";
        int textWidth = MeasureText(text, 20);
        DrawText(text, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2, 20, WHITE);
        if (IsKeyPressed(KEY_ESCAPE)) {
            GameStateManager::getInstance().changeState("main_menu", GameStateManager::Transition::FADE);
        }
    }
}

bool GameScreen::isFinished() const {
    return _finished;
}

void GameScreen::triggerGameEnd(const std::string& winningTeam) {
    EndScreen* endScreen = static_cast<EndScreen*>(
        GameStateManager::getInstance().getStateInstance("end_screen"));
    if (endScreen) {
        endScreen->setWinningTeam(winningTeam);
        GameStateManager::getInstance().changeState("end_screen", 
            GameStateManager::Transition::FADE);
    }
}
// Updated: 2025-05-31 00:53:46
