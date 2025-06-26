#ifndef DASHBOARD_HPP_
#define DASHBOARD_HPP_

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include "raylib.h"
#include "AComponent.hpp"

struct ResourceStats {
    int food = 0;
    int linemate = 0;
    int deraumere = 0;
    int sibur = 0;
    int mendiane = 0;
    int phiras = 0;
    int thystame = 0;
};

struct TeamStats {
    std::string name;
    int playerCount = 0;
    int highestLevel = 1;
    int totalLevels = 0;
    float averageLevel = 1.0f;
    std::vector<int> playerLevels;
    Color teamColor;
    float growthRate = 0.0f;
    int activePlayers = 0;
};

struct MetricCard {
    std::string title;
    std::string value;
    std::string change;
    bool isPositive;
    Color accentColor;
    std::string icon;
};

class Dashboard : public AComponent {
public:
    Dashboard();
    ~Dashboard();

    void update(float dt) override;
    void draw() const override;
    void toggle();
    bool isVisible() const { return _visible; }
    void setVisible(bool visible) { _visible = visible; }

private:
    bool _visible = false;
    float _refreshTimer = 0.0f;
    float _animationTimer = 0.0f;
    float _cardHoverStates[8] = {0};
    static constexpr float REFRESH_INTERVAL = 0.5f;
    
    ResourceStats _globalResources;
    ResourceStats _previousResources;
    std::map<std::string, TeamStats> _teamStats;
    std::map<std::string, TeamStats> _previousTeamStats;
    int _totalPlayers = 0;
    int _mapWidth = 0;
    int _mapHeight = 0;
    float _gameTime = 0.0f;
    
    std::vector<MetricCard> _metricCards;
    
    void updateStats();
    void collectResourceStats();
    void collectTeamStats();
    void updateMetricCards();
    
    void drawModernBackground() const;
    void drawHeader() const;
    void drawMetricCards() const;
    void drawResourceChart() const;
    void drawTeamLeaderboard() const;
    void drawActivityFeed() const;
    void drawSystemInfo() const;
    
    void drawCard(Rectangle rect, const char* title, std::function<void()> content, int = 0) const;
    void drawGlassCard(Rectangle rect, Color tint) const;
    void drawMetricCard(Rectangle rect, const MetricCard& card, int index) const;
    void drawProgressRing(Vector2 center, float radius, float progress, Color color) const;
    void drawSparkline(Rectangle rect, const std::vector<float>& data, Color color) const;
    
    Color getAccentColor(const std::string& type) const;
    std::string formatNumber(int number) const;
    std::string formatTime(float seconds) const;
    std::string getChangeString(int current, int previous) const;
    float smoothStep(float t) const;
    
    TeamStats* findHighestLevelTeam();
    
    static constexpr Color DARK_BG = {15, 15, 20, 255};
    static constexpr Color CARD_BG = {25, 25, 35, 255};
    static constexpr Color HOVER_BG = {35, 35, 45, 255};
    static constexpr Color BORDER_COLOR = {45, 45, 55, 255};
    static constexpr Color TEXT_PRIMARY = {240, 240, 245, 255};
    static constexpr Color TEXT_SECONDARY = {160, 160, 170, 255};
    static constexpr Color ACCENT_GREEN = {16, 185, 129, 255};
    static constexpr Color ACCENT_BLUE = {59, 130, 246, 255};
    static constexpr Color ACCENT_PURPLE = {147, 51, 234, 255};
    static constexpr Color ACCENT_ORANGE = {251, 146, 60, 255};
};

#endif
