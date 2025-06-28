---
sidebar_position: 1
---

# Game Rules and Mechanics

## Objective

Zappy is a survival and evolution game where teams of AI players compete to achieve the highest level of evolution. The first team to have 6 players reach level 8 wins the game.

## World Structure

### Map Layout
- **Shape**: Rectangular torus (edges wrap around)
- **Dimensions**: Configurable (minimum 6x6, maximum 50x50)
- **Tiles**: Each tile can contain players and resources

### Resource Types

| Resource | Symbol | Purpose |
|----------|--------|---------|
| Food | 🍎 | Sustains player life |
| Linemate | 💎 | Required for level 1-2 elevation |
| Deraumere | 🔮 | Required for level 2-3 elevation |
| Sibur | ⚪ | Required for level 3-4 elevation |
| Mendiane | 🟡 | Required for level 4-5 elevation |
| Phiras | 🔵 | Required for level 5-6 elevation |
| Thystame | 🟣 | Required for level 6-7 elevation |

### Resource Distribution
Resources spawn randomly across the map with different density rates:
- **Food**: 50% density (most common)
- **Linemate**: 30% density
- **Deraumere**: 15% density
- **Sibur**: 10% density
- **Mendiane**: 10% density
- **Phiras**: 8% density
- **Thystame**: 5% density (rarest)

## Player Mechanics

### Life System
- **Initial Life**: 10 food units (1260 time units of survival)
- **Food Consumption**: 1 food unit per 126 time units
- **Death**: Occurs when food reaches 0
- **Respawn**: New players can join if team slots available

### Movement and Orientation
- **Directions**: North (1), East (2), South (3), West (4)
- **Vision**: Limited to nearby tiles based on current level
- **Movement**: One tile per action in current facing direction

### Inventory Management
- **Capacity**: Unlimited resource storage
- **Actions**: Pick up and drop resources on current tile
- **Sharing**: Resources can be dropped for team members

## Level Advancement

### Elevation Requirements

| Level | Players | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
|-------|---------|----------|-----------|-------|----------|--------|----------|
| 1→2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2→3 | 1 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3→4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4→5 | 2 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5→6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6→7 | 4 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7→8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

### Elevation Process
1. **Gathering**: Collect required players and resources on same tile
2. **Initiation**: Lead player starts incantation
3. **Duration**: Elevation takes 300 time units
4. **Interruption**: Other players can disrupt the process
5. **Success**: All participating players advance one level

## Player Actions

### Basic Commands

| Action | Time Cost | Description |
|--------|-----------|-------------|
| Forward | 7 time units | Move one tile forward |
| Right | 7 time units | Turn 90° clockwise |
| Left | 7 time units | Turn 90° counter-clockwise |
| Look | 7 time units | View surrounding tiles |
| Inventory | 1 time unit | Check personal resources |
| Take | 7 time units | Pick up resource from tile |
| Set | 7 time units | Drop resource on tile |
| Eject | 7 time units | Push other players from tile |
| Broadcast | 7 time units | Send message to team |
| Fork | 42 time units | Create new player slot |
| Incantation | 300 time units | Attempt level advancement |

### Vision System
Players can see tiles in their field of view:
- **Level 1**: 3 tiles in front
- **Level 2**: 5 tiles in front  
- **Level 3**: 7 tiles in front
- **Higher levels**: Extended vision range

## Team Dynamics

### Communication
- **Broadcast Range**: Messages reach entire team
- **Sound Direction**: Receivers get directional information
- **Message Limit**: No limit on broadcast frequency

### Coordination Strategies
- **Resource Sharing**: Drop items for teammates
- **Elevation Support**: Gather for group ceremonies
- **Exploration**: Spread out to map resources
- **Defense**: Protect elevation sites from enemies

## Winning Conditions

### Victory Requirements
- **Primary Goal**: First team with 6 players at level 8
- **Alternative**: Last team standing if others are eliminated
- **Time Limit**: Games may have maximum duration limits

### Game End Events
- Team achieves victory condition
- Server administrator ends game
- All teams eliminated (draw condition)

## Strategy Considerations

### Early Game
- Focus on food collection for survival
- Map exploration to locate resources
- Establish territorial control

### Mid Game  
- Resource accumulation for elevations
- Team coordination for group ceremonies
- Interference with enemy advancement

### Late Game
- Mass elevation attempts
- Defensive positioning
- Resource denial tactics
