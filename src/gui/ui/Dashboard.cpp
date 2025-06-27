#include "Dashboard.hpp"
#include "../core/FontManager.hpp"
#include "../core/GameWorld.hpp"
#include "../entities/CharacterManager.hpp"
#include "../core/Constants.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>
#include <cmath>

Dashboard::Dashboard() : AComponent(Vector2{0, 0}, Vector2{0, 0}) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    _position = Vector2{0, 0};
    _size = Vector2{(float)screenW, (float)screenH};
    
    for (int i = 0; i < 8; i++) {
        _cardHoverStates[i] = 0.0f;
    }
}

Dashboard::~Dashboard() = default;

void Dashboard::update(float dt) {
    if (!_visible) return;
    
    _animationTimer += dt;
    _refreshTimer += dt;
    
    if (_refreshTimer >= REFRESH_INTERVAL) {
        updateStats();
        _refreshTimer = 0.0f;
    }
    
    _gameTime += dt;
    
    Vector2 mousePos = GetMousePosition();
    
    float startY = 140;
    float totalWidth = GetScreenWidth() - 100;
    float cardWidth = (totalWidth - 60) / 4.0f;
    
    for (int i = 0; i < 4; i++) {
        float x = 50 + i * (cardWidth + 20);
        Rectangle cardRect = {x, startY, cardWidth, 120};
        bool isHovered = CheckCollisionPointRec(mousePos, cardRect);
        
        float target = isHovered ? 1.0f : 0.0f;
        _cardHoverStates[i] += (target - _cardHoverStates[i]) * dt * 8.0f;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        toggle();
    }
    
    if (IsKeyPressed(KEY_F7)) {
        toggle();
    }
}

void Dashboard::toggle() {
    _visible = !_visible;
    if (_visible) {
        updateStats();
        _animationTimer = 0.0f;
    }
}

void Dashboard::updateStats() {
    _previousResources = _globalResources;
    _previousTeamStats = _teamStats;
    
    collectResourceStats();
    collectTeamStats();
    updateMetricCards();
}

void Dashboard::collectResourceStats() {
    _globalResources = ResourceStats{};
    
    auto& gameWorld = GameWorld::getInstance();
    
    _mapWidth = gameWorld.getPlayableWidth();
    _mapHeight = gameWorld.getPlayableHeight();
    
    for (int x = 1; x <= _mapWidth; x++) {
        for (int y = 1; y <= _mapHeight; y++) {
            auto tile = gameWorld.getTileAt(x, y);
            _globalResources.food += tile.resources.food;
            _globalResources.linemate += tile.resources.linemate;
            _globalResources.deraumere += tile.resources.deraumere;
            _globalResources.sibur += tile.resources.sibur;
            _globalResources.mendiane += tile.resources.mendiane;
            _globalResources.phiras += tile.resources.phiras;
            _globalResources.thystame += tile.resources.thystame;
        }
    }
}

void Dashboard::collectTeamStats() {
    _teamStats.clear();
    _totalPlayers = 0;
    
    auto& charManager = CharacterManager::getInstance();
    auto characters = charManager.getAllCharacters();
    
    for (const auto& character : characters) {
        if (!character) continue;
        
        std::string teamName = character->getTeam();
        int level = character->getLevel();
        
        if (_teamStats.find(teamName) == _teamStats.end()) {
            _teamStats[teamName] = TeamStats{};
            _teamStats[teamName].name = teamName;
            _teamStats[teamName].teamColor = charManager.getTeamColor(teamName);
        }
        
        auto& team = _teamStats[teamName];
        team.playerCount++;
        team.playerLevels.push_back(level);
        team.totalLevels += level;
        team.highestLevel = std::max(team.highestLevel, level);
        team.activePlayers++;
        
        _totalPlayers++;
    }
    
    for (auto& [teamName, team] : _teamStats) {
        if (team.playerCount > 0) {
            team.averageLevel = (float)team.totalLevels / (float)team.playerCount;
            
            auto prevTeam = _previousTeamStats.find(teamName);
            if (prevTeam != _previousTeamStats.end()) {
                float prevAvg = prevTeam->second.averageLevel;
                team.growthRate = ((team.averageLevel - prevAvg) / prevAvg) * 100.0f;
            }
        }
    }
}

void Dashboard::updateMetricCards() {
    _metricCards.clear();
    
    int totalResources = _globalResources.food + _globalResources.linemate + 
                        _globalResources.deraumere + _globalResources.sibur +
                        _globalResources.mendiane + _globalResources.phiras + 
                        _globalResources.thystame;
    
    int prevTotal = _previousResources.food + _previousResources.linemate + 
                   _previousResources.deraumere + _previousResources.sibur +
                   _previousResources.mendiane + _previousResources.phiras + 
                   _previousResources.thystame;
    
    _metricCards.push_back({
        "Total Resources", formatNumber(totalResources), 
        getChangeString(totalResources, prevTotal), 
        totalResources >= prevTotal, ACCENT_GREEN, "RES"
    });
    
    _metricCards.push_back({
        "Active Players", formatNumber(_totalPlayers),
        getChangeString(_totalPlayers, _totalPlayers > 0 ? _totalPlayers - 1 : 0), 
        true, ACCENT_BLUE, "PLY"
    });
    
    _metricCards.push_back({
        "Active Teams", formatNumber((int)_teamStats.size()),
        getChangeString((int)_teamStats.size(), (int)_previousTeamStats.size()), 
        (int)_teamStats.size() >= (int)_previousTeamStats.size(), ACCENT_PURPLE, "TEA"
    });
    
    auto* leadingTeam = findHighestLevelTeam();
    _metricCards.push_back({
        "Highest Level", leadingTeam ? formatNumber(leadingTeam->highestLevel) : "0",
        leadingTeam ? "+" + formatNumber(leadingTeam->highestLevel - 1) : "0%", 
        true, ACCENT_ORANGE, "LVL"
    });
}

void Dashboard::draw() const {
    if (!_visible) return;
    
    drawModernBackground();
    drawHeader();
    drawMetricCards();
    drawResourceChart();
    drawTeamLeaderboard();
    drawSystemInfo();
}

void Dashboard::drawModernBackground() const {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARK_BG, 0.95f));
    
    for (int i = 0; i < 20; i++) {
        float x = (i * GetScreenWidth() / 20.0f) + sin(_animationTimer + i) * 30;
        float y = sin(_animationTimer * 0.5f + i * 0.3f) * 20 + GetScreenHeight() * 0.5f;
        DrawCircleGradient(x, y, 2, Fade(ACCENT_BLUE, 0.1f), Fade(ACCENT_BLUE, 0.0f));
    }
}

void Dashboard::drawHeader() const {
    Font titleFont = FontManager::getInstance().getFont("bold");
    Font font = FontManager::getInstance().getFont("medium");
    
    float headerY = 30;
    
    DrawTextEx(titleFont, "Zappy Analytics", {50, headerY}, 36, 2, TEXT_PRIMARY);
    
    const char* subtitle = "Real-time game monitoring dashboard";
    DrawTextEx(font, subtitle, {50, headerY + 50}, 18, 1, TEXT_SECONDARY);
    
    float closeX = GetScreenWidth() - 220;
    Rectangle closeBtn = {closeX, headerY, 150, 40};
    drawGlassCard(closeBtn, Fade(ACCENT_BLUE, 0.1f));
    
    const char* closeText = "Press ESC to close";
    Vector2 closeSize = MeasureTextEx(font, closeText, 16, 1);
    DrawTextEx(font, closeText, 
        {closeBtn.x + (closeBtn.width - closeSize.x) / 2, closeBtn.y + (closeBtn.height - closeSize.y) / 2}, 
        16, 1, TEXT_SECONDARY);
    
}

void Dashboard::drawMetricCards() const {
    float startY = 140;
    float totalWidth = GetScreenWidth() - 100;
    float cardWidth = (totalWidth - 60) / 4.0f;
    
    for (size_t i = 0; i < _metricCards.size() && i < 4; i++) {
        float x = 50 + i * (cardWidth + 20);
        Rectangle cardRect = {x, startY, cardWidth, 120};
        drawMetricCard(cardRect, _metricCards[i], i);
    }
}

void Dashboard::drawMetricCard(Rectangle rect, const MetricCard& card, int index) const {
    float hoverAmount = _cardHoverStates[index];
    float scale = 1.0f + hoverAmount * 0.02f;
    
    Rectangle scaledRect = {
        rect.x - (rect.width * (scale - 1.0f)) / 2,
        rect.y - (rect.height * (scale - 1.0f)) / 2,
        rect.width * scale,
        rect.height * scale
    };
    
    Color cardBg = ColorLerp(CARD_BG, HOVER_BG, hoverAmount);
    drawGlassCard(scaledRect, cardBg);
    
    Font font = FontManager::getInstance().getFont("medium");
    Font titleFont = FontManager::getInstance().getFont("bold");
    
    float padding = 20;
    
    DrawCircle(rect.x + padding + 15, rect.y + padding + 15, 18, Fade(card.accentColor, 0.2f));
    
    Font boldFont = FontManager::getInstance().getFont("bold");
    Vector2 iconSize = MeasureTextEx(boldFont, card.icon.c_str(), 16, 1);
    DrawTextEx(boldFont, card.icon.c_str(), 
        {rect.x + padding + 15 - iconSize.x/2, rect.y + padding + 15 - iconSize.y/2}, 
        16, 1, card.accentColor);
    
    DrawTextEx(font, card.title.c_str(), {rect.x + padding, rect.y + padding + 40}, 16, 1, TEXT_SECONDARY);
    
    DrawTextEx(titleFont, card.value.c_str(), {rect.x + padding, rect.y + padding + 60}, 32, 2, TEXT_PRIMARY);
    
    Color changeColor = card.isPositive ? ACCENT_GREEN : Color{239, 68, 68, 255};
    DrawTextEx(font, card.change.c_str(), {rect.x + rect.width - padding - 80, rect.y + padding + 65}, 15, 1, changeColor);
    
    if (hoverAmount > 0.5f) {
        float glowAlpha = (hoverAmount - 0.5f) * 2.0f;
        DrawRectangleRounded(scaledRect, 0.15f, 10, Fade(card.accentColor, glowAlpha * 0.1f));
    }
}

void Dashboard::drawResourceChart() const {
    Rectangle chartRect = {50, 300, GetScreenWidth() * 0.6f - 75, 250};
    
    drawCard(chartRect, "Resource Distribution", [this, chartRect]() {
        Font font = FontManager::getInstance().getFont("medium");
        
        struct ResourceData {
            const char* name;
            int value;
            Color color;
        };
        
        ResourceData resources[] = {
            {"Food", _globalResources.food, {255, 193, 7, 255}},
            {"Linemate", _globalResources.linemate, {255, 152, 0, 255}},
            {"Deraumere", _globalResources.deraumere, {156, 39, 176, 255}},
            {"Sibur", _globalResources.sibur, {3, 169, 244, 255}},
            {"Mendiane", _globalResources.mendiane, {233, 30, 99, 255}},
            {"Phiras", _globalResources.phiras, {76, 175, 80, 255}},
            {"Thystame", _globalResources.thystame, {121, 85, 72, 255}}
        };
        
        int total = 0;
        for (int i = 0; i < 7; i++) total += resources[i].value;
        
        float barWidth = (chartRect.width - 80) / 7.0f - 10;
        float maxHeight = chartRect.height - 100;
        
        for (int i = 0; i < 7; i++) {
            if (total == 0) continue;
            
            float height = (float)resources[i].value / total * maxHeight;
            float x = chartRect.x + 40 + i * (barWidth + 10);
            float y = chartRect.y + chartRect.height - 60 - height;
            
            Rectangle bar = {x, y, barWidth, height};
            DrawRectangleRounded(bar, 0.2f, 10, resources[i].color);
            
            Vector2 textSize = MeasureTextEx(font, resources[i].name, 16, 1);
            DrawTextEx(font, resources[i].name, 
                {x + (barWidth - textSize.x) / 2, chartRect.y + chartRect.height - 40}, 
                18, 1, TEXT_SECONDARY);
            
            std::string valueStr = std::to_string(resources[i].value);
            Vector2 valueSize = MeasureTextEx(font, valueStr.c_str(), 14, 1);
            DrawTextEx(font, valueStr.c_str(),
                {x + (barWidth - valueSize.x) / 2, y - 25},
                16, 1, TEXT_PRIMARY);
        }
    }, 4);
}

void Dashboard::drawTeamLeaderboard() const {
    Rectangle leaderRect = {GetScreenWidth() * 0.6f + 25, 300, GetScreenWidth() * 0.4f - 75, 250};
    
    drawCard(leaderRect, "Team Leaderboard", [this, leaderRect]() {
        Font font = FontManager::getInstance().getFont("medium");
        
        std::vector<std::pair<std::string, TeamStats>> sortedTeams;
        for (const auto& team : _teamStats) {
            sortedTeams.push_back(team);
        }
        
        std::sort(sortedTeams.begin(), sortedTeams.end(),
            [](const auto& a, const auto& b) {
                return a.second.highestLevel > b.second.highestLevel;
            });
        
        float itemHeight = 45;
        float startY = leaderRect.y + 50;
        
        for (size_t i = 0; i < sortedTeams.size() && i < 5; i++) {
            const auto& team = sortedTeams[i].second;
            float y = startY + i * itemHeight;
            
            std::string rank = std::to_string(i + 1);
            DrawTextEx(font, rank.c_str(), {leaderRect.x + 20, y + 3.0f}, 18, 1, TEXT_SECONDARY);
            
            DrawCircle(leaderRect.x + 60, y + 12, 10, team.teamColor);
            
            std::string teamText = team.name + " - (" + std::to_string(team.playerCount) + " players)";
            DrawTextEx(font, teamText.c_str(), {leaderRect.x + 85, y + 4.0f}, 16, 1, TEXT_PRIMARY);
            
            std::string levelText = "Lv." + std::to_string(team.highestLevel);
            Vector2 levelSize = MeasureTextEx(font, levelText.c_str(), 16, 1);
            DrawTextEx(font, levelText.c_str(),
                {leaderRect.x + leaderRect.width - 50 - levelSize.x, y},
                16, 1, ACCENT_GREEN);
        }
    }, 5);
}

void Dashboard::drawSystemInfo() const {
    Rectangle infoRect = {50, 580, (float)(GetScreenWidth() - 100), 110};
    
    drawCard(infoRect, "System Information", [this, infoRect]() {
        Font font = FontManager::getInstance().getFont("medium");
        
        float leftX = infoRect.x + 30;
        float rightX = infoRect.x + infoRect.width / 2 + 30;
        float topY = infoRect.y + 50;
        
        char mapInfo[64];
        snprintf(mapInfo, sizeof(mapInfo), "Map: %dx%d", _mapWidth, _mapHeight);
        DrawTextEx(font, mapInfo, {leftX, topY}, 16, 1, TEXT_PRIMARY);
        
        std::string timeStr = formatTime(_gameTime);
        char timeInfo[64];
        snprintf(timeInfo, sizeof(timeInfo), "Uptime: %s", timeStr.c_str());
        DrawTextEx(font, timeInfo, {rightX, topY}, 16, 1, TEXT_PRIMARY);
        
        char serverInfo[64];
        snprintf(serverInfo, sizeof(serverInfo), "FPS: %d", GetFPS());
        DrawTextEx(font, serverInfo, {leftX, topY + 30}, 16, 1, TEXT_SECONDARY);
        
        char memInfo[64];
        snprintf(memInfo, sizeof(memInfo), "Players: %d", _totalPlayers);
        DrawTextEx(font, memInfo, {rightX, topY + 30}, 16, 1, TEXT_SECONDARY);
    }, 6);
}

void Dashboard::drawCard(Rectangle rect, const char* title, std::function<void()> content, int) const {
    drawGlassCard(rect, CARD_BG);
    
    Font font = FontManager::getInstance().getFont("medium");
    DrawTextEx(font, title, {rect.x + 20, rect.y + 15}, 18, 1, TEXT_PRIMARY);
    content();
}

void Dashboard::drawGlassCard(Rectangle rect, Color tint) const {
    DrawRectangleRounded(rect, 0.15f, 10, tint);
    DrawRectangleRoundedLines(rect, 0.15f, 10, BORDER_COLOR);
}

Color Dashboard::getAccentColor(const std::string& type) const {
    if (type == "food") return Color{255, 193, 7, 255};
    if (type == "linemate") return Color{255, 152, 0, 255};
    if (type == "deraumere") return Color{156, 39, 176, 255};
    if (type == "sibur") return Color{3, 169, 244, 255};
    if (type == "mendiane") return Color{233, 30, 99, 255};
    if (type == "phiras") return Color{76, 175, 80, 255};
    if (type == "thystame") return Color{121, 85, 72, 255};
    return ACCENT_BLUE;
}

std::string Dashboard::formatNumber(int number) const {
    if (number >= 1000000) {
        return std::to_string(number / 1000000) + "M";
    } else if (number >= 1000) {
        return std::to_string(number / 1000) + "K";
    }
    return std::to_string(number);
}

std::string Dashboard::formatTime(float seconds) const {
    int hours = (int)seconds / 3600;
    int minutes = ((int)seconds % 3600) / 60;
    int secs = (int)seconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;
    return oss.str();
}

std::string Dashboard::getChangeString(int current, int previous) const {
    if (previous == 0) return "+100%";
    
    float change = ((float)(current - previous) / previous) * 100.0f;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    if (change > 0) oss << "+";
    oss << change << "%";
    return oss.str();
}

float Dashboard::smoothStep(float t) const {
    return t * t * (3.0f - 2.0f * t);
}

TeamStats* Dashboard::findHighestLevelTeam() {
    TeamStats* highest = nullptr;
    int maxLevel = 0;
    
    for (auto& [teamName, team] : _teamStats) {
        if (team.highestLevel > maxLevel) {
            maxLevel = team.highestLevel;
            highest = &team;
        }
    }
    
    return highest;
}
