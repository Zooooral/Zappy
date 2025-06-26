/*
** EPITECH PROJECT, 2025
** B-YEP-400-PAR-4-1-zappy-maxence.bunel [WSL: Ubuntu]
** File description:
** GameScreen
*/

#include <iostream>
#include <sstream>

#include "GameScreen.hpp"
#include "../ui/Dashboard.hpp"
#include "../entities/EggManager.hpp"
#include "EndScreen.hpp"
#include "../core/GameWorld.hpp"
#include "../core/CameraController.hpp"
#include "../core/DebugSystem.hpp"
#include "../core/GameStateManager.hpp"
#include "../entities/CharacterManager.hpp"
#include "../core/SoundManager.hpp"
#include "../network/NetworkManager.hpp"
#include "../core/ConfigManager.hpp"
#include "../core/ChatSystem.hpp"

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

    _dashboard = std::make_unique<Dashboard>();

    _mapInitialized = false;
    _updateTimer = 0.0f;
    _nextPlayerId = 1;
}

GameScreen::~GameScreen() {
}

void GameScreen::onEnter() {
    setTransitionAlpha(1.0f);
    _finished = false;
    _shouldReturn = false;
    _mapInitialized = false;
    _updateTimer = 0.0f;
    _nextPlayerId = 1;
    _activePlayerIds.clear();

    CharacterManager::getInstance().initialize();
    EggManager::getInstance().cleanup();

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
    SoundManager::getInstance().stopAllSounds();
}

void GameScreen::setupNetworkAndRequestData() {
    NetworkManager& network = NetworkManager::getInstance();

    if (!network.isConnected() || network.getConnectionState() != ConnectionState::AUTHENTICATED) {
        std::cerr << "Error: No authenticated connection available" << std::endl;
        GameStateManager::getInstance().changeState("main_menu", GameStateManager::Transition::FADE);
        return;
    }

    network.setMessageCallback([this](const std::string& command) {
        handleServerCommand(command);
    });

    requestInitialGameData();
}

void GameScreen::requestInitialGameData() {
    NetworkManager& network = NetworkManager::getInstance();

    std::cout << "[DEBUG] Requesting initial game data" << std::endl;
    network.sendCommand("msz");
    network.sendCommand("mct");
    network.sendCommand("tna");
}

void GameScreen::requestPlayerUpdates() {
    NetworkManager& network = NetworkManager::getInstance();

    std::cout << "[DEBUG] Requesting updates for " << _activePlayerIds.size() << " active players" << std::endl;
    for (int id : _activePlayerIds) {
        network.sendCommand("ppo #" + std::to_string(id));
        network.sendCommand("plv #" + std::to_string(id));
        network.sendCommand("pin #" + std::to_string(id));
    }
}

void GameScreen::handleServerCommand(const std::string& command) {
    if (command.empty()) return;
    
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    std::cout << "[DEBUG] Handling command: " << cmd << " (full: " << command << ")" << std::endl;
    
    if (cmd == "msz") {
        int width, height;
        if (iss >> width >> height) {
            std::cout << "[DEBUG] Map size: " << width << "x" << height << std::endl;
            GameWorld::getInstance().initialize(width, height);
        }
    } else if (cmd == "bct") {
        int x, y;
        std::vector<int> resources(7);
        if (iss >> x >> y) {
            for (int i = 0; i < 7; i++) {
                iss >> resources[i];
            }
            GameWorld::TileResources tileRes;
            tileRes.food = resources[0];
            tileRes.linemate = resources[1];
            tileRes.deraumere = resources[2];
            tileRes.sibur = resources[3];
            tileRes.mendiane = resources[4];
            tileRes.phiras = resources[5];
            tileRes.thystame = resources[6];
            GameWorld::getInstance().updateTileResources(x + 1, y + 1, tileRes);
            if (!_mapInitialized && x == 0 && y == 0) {
                _mapInitialized = true;
                std::cout << "[DEBUG] Map initialization complete" << std::endl;
            }
        }
    } else if (cmd == "tna") {
        std::string team;
        if (iss >> team) {
            std::cout << "[DEBUG] Team found: " << team << std::endl;
        }
    } else if (cmd == "pnw") {
        std::string idStr;
        int x, y, orientation, level;
        std::string team;
        if (iss >> idStr >> x >> y >> orientation >> level >> team) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] New player #" << id << " at (" << x << "," << y << ") team " << team << std::endl;
                CharacterManager::getInstance().addCharacter(id, Vector3{(float)(x + 1), 1.0f, (float)(y + 1)}, team, level);
                Character* character = CharacterManager::getInstance().getCharacter(id);
                if (character) {
                    character->setOrientation(orientation);
                }
                _activePlayerIds.insert(id);
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
                    character->setTargetPosition(Vector3{(float)(x + 1), 1.0f, (float)(y + 1)});
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
        int x, y;
        std::vector<int> inventory(7);
        if (iss >> idStr >> x >> y) {
            for (int i = 0; i < 7; i++) {
                iss >> inventory[i];
            }
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                Character* character = CharacterManager::getInstance().getCharacter(id);
                if (character) {
                    CharacterInventory inv;
                    inv.food = inventory[0];
                    inv.linemate = inventory[1];
                    inv.deraumere = inventory[2];
                    inv.sibur = inventory[3];
                    inv.mendiane = inventory[4];
                    inv.phiras = inventory[5];
                    inv.thystame = inventory[6];
                    character->setInventory(inv);
                }
            }
        }
    } else if (cmd == "pdi") {
        std::string idStr;
        if (iss >> idStr) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " died" << std::endl;
                CharacterManager::getInstance().removeCharacter(id);
                _activePlayerIds.erase(id);
            }
        }
    } else if (cmd == "pex") {
        std::string idStr;
        if (iss >> idStr) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " was expelled" << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Player #" + std::to_string(id) + " was expelled", ORANGE);
            }
        }
    } else if (cmd == "pbc") {
        std::string idStr, message;
        if (iss >> idStr) {
            std::getline(iss, message);
            if (!message.empty() && message[0] == ' ') message = message.substr(1);
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " broadcasted: " << message << std::endl;
                ChatSystem::getInstance().addMessage("Broadcast #" + std::to_string(id), message, SKYBLUE);
            }
        }
    } else if (cmd == "pic") {
        int x, y, level;
        if (iss >> x >> y >> level) {
            std::cout << "[DEBUG] Incantation started at (" << x << "," << y << ") level " << level << std::endl;
            ChatSystem::getInstance().addMessage("Incantation", "Started at (" + std::to_string(x) + "," + std::to_string(y) + ") level " + std::to_string(level), PURPLE);
            
            std::string playerIdStr;
            while (iss >> playerIdStr) {
                if (playerIdStr.front() == '#') {
                    int playerId = std::stoi(playerIdStr.substr(1));
                    Character* character = CharacterManager::getInstance().getCharacter(playerId);
                    if (character) {
                        character->setElevating(true);
                        std::cout << "[DEBUG] Player #" << playerId << " started elevating" << std::endl;
                    }
                }
            }
        }
    } else if (cmd == "pie") {
        int x, y;
        std::string result;
        if (iss >> x >> y >> result) {
            std::cout << "[DEBUG] Incantation ended at (" << x << "," << y << ") result: " << result << std::endl;
            Color color = (result == "ok") ? GREEN : RED;
            ChatSystem::getInstance().addMessage("Incantation", "Ended at (" + std::to_string(x) + "," + std::to_string(y) + ") " + result, color);
            
            for (auto character : CharacterManager::getInstance().getAllCharacters()) {
                if (character->isElevating() && 
                    (int)character->getTilePosition().x == x && 
                    (int)character->getTilePosition().y == y) {
                    character->setElevating(false);
                    std::cout << "[DEBUG] Player #" << character->getId() << " stopped elevating" << std::endl;
                }
            }
        }
    } else if (cmd == "pfk") {
        std::string idStr;
        if (iss >> idStr) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " is laying an egg" << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Player #" + std::to_string(id) + " is laying an egg", YELLOW);
            }
        }
    } else if (cmd == "pdr") {
        std::string idStr;
        int resource;
        if (iss >> idStr >> resource) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " dropped resource " << resource << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Player #" + std::to_string(id) + " dropped resource", GRAY);
            }
        }
    } else if (cmd == "pgt") {
        std::string idStr;
        int resource;
        if (iss >> idStr >> resource) {
            if (idStr.front() == '#') {
                int id = std::stoi(idStr.substr(1));
                std::cout << "[DEBUG] Player #" << id << " collected resource " << resource << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Player #" + std::to_string(id) + " collected resource", GREEN);
            }
        }
    } else if (cmd == "enw") {
        std::string eggIdStr, playerIdStr;
        int x, y;
        if (iss >> eggIdStr >> playerIdStr >> x >> y) {
            if (eggIdStr.front() == '#' && playerIdStr.front() == '#') {
                int eggId = std::stoi(eggIdStr.substr(1));
                int playerId = std::stoi(playerIdStr.substr(1));
                std::cout << "[DEBUG] Egg #" << eggId << " laid by player #" << playerId << " at (" << x << "," << y << ")" << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Egg #" + std::to_string(eggId) + " laid at (" + std::to_string(x) + "," + std::to_string(y) + ")", YELLOW);
                EggManager::getInstance().addEgg(eggId, x, y, playerId);
            }
        }
    } else if (cmd == "eht") {
        std::string eggIdStr;
        if (iss >> eggIdStr) {
            if (eggIdStr.front() == '#') {
                int eggId = std::stoi(eggIdStr.substr(1));
                std::cout << "[DEBUG] Egg #" << eggId << " is hatching" << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Egg #" + std::to_string(eggId) + " is hatching", ORANGE);
                EggManager::getInstance().setEggHatching(eggId, true);
            }
        }
    } else if (cmd == "ebo") {
        std::string eggIdStr;
        if (iss >> eggIdStr) {
            if (eggIdStr.front() == '#') {
                int eggId = std::stoi(eggIdStr.substr(1));
                std::cout << "[DEBUG] Player connected for egg #" << eggId << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Player connected for egg #" + std::to_string(eggId), GREEN);
                EggManager::getInstance().removeEgg(eggId);
            }
        }
    } else if (cmd == "edi") {
        std::string eggIdStr;
        if (iss >> eggIdStr) {
            if (eggIdStr.front() == '#') {
                int eggId = std::stoi(eggIdStr.substr(1));
                std::cout << "[DEBUG] Egg #" << eggId << " died" << std::endl;
                ChatSystem::getInstance().addMessage("Game", "Egg #" + std::to_string(eggId) + " died", RED);
                EggManager::getInstance().removeEgg(eggId);
            }
        }
    } else if (cmd == "sgt") {
        int timeUnit;
        if (iss >> timeUnit) {
            std::cout << "[DEBUG] Time unit: " << timeUnit << std::endl;
        }
    } else if (cmd == "sst") {
        int timeUnit;
        if (iss >> timeUnit) {
            std::cout << "[DEBUG] Time unit set to: " << timeUnit << std::endl;
            ChatSystem::getInstance().addMessage("Server", "Time unit set to " + std::to_string(timeUnit), BLUE);
        }
    } else if (cmd == "seg") {
        std::string team;
        if (iss >> team) {
            std::cout << "[DEBUG] Game ended, winner: " << team << std::endl;
            triggerGameEnd(team);
        }
    } else if (cmd == "smg") {
        std::string message;
        std::getline(iss, message);
        if (!message.empty() && message[0] == ' ') message = message.substr(1);
        std::cout << "[DEBUG] Server message: " << message << std::endl;
        ChatSystem::getInstance().addMessage("Server", message, BLUE);
    } else if (cmd == "suc") {
        std::cout << "[DEBUG] Server sent success response!" << std::endl;
    } else if (cmd == "sbp") {
        std::cout << "[DEBUG] Server sent bad parameter response!" << std::endl;
        ChatSystem::getInstance().addMessage("Error", "Bad parameter sent to server", RED);
    } else {
        std::cout << "[DEBUG] Unhandled command: " << cmd << " (full: " << command << ")" << std::endl;
        ChatSystem::getInstance().addMessage("Debug", "Unhandled: " + command, GRAY);
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
    _dashboard->update(dt);

    _updateTimer += dt;
    if (_updateTimer >= 2.0f && _mapInitialized) {
        requestPlayerUpdates();
        _updateTimer = 0.0f;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (_dashboard->isVisible()) {
            _dashboard->toggle();
        } else if (_inventoryUI->isVisible()) {
            _inventoryUI->setVisible(false);
        }
    }

    if (IsKeyPressed(KEY_F7)) {
        _dashboard->toggle();
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
    _dashboard->draw();

    ConnectionState connectionState = NetworkManager::getInstance().getConnectionState();
    const char* stateText = "Disconnected";
    Color stateColor = RED;

    switch (connectionState) {
        case ConnectionState::CONNECTING:
            stateText = "Connecting...";
            stateColor = YELLOW;
            break;
        case ConnectionState::CONNECTED:
            stateText = "Connected";
            stateColor = ORANGE;
            break;
        case ConnectionState::AUTHENTICATED:
            stateText = "Authenticated";
            stateColor = GREEN;
            break;
        case ConnectionState::ERROR:
            stateText = "Connection Error";
            stateColor = RED;
            break;
        case ConnectionState::DISCONNECTED:
        default:
            stateText = "Disconnected";
            stateColor = RED;
            break;
    }

    int screenHeight = GetScreenHeight();
    DrawText(stateText, 10, screenHeight - 30, 20, stateColor);

    if (_shouldReturn) {
        _finished = true;
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
