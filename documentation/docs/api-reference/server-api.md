# Server API Protocol

## Table of Contents
1. [Overview](#overview)
2. [Connection Protocol](#connection-protocol)
3. [Command System](#command-system)
4. [Player Commands](#player-commands)
5. [Game Mechanics](#game-mechanics)
6. [Vision System](#vision-system)
7. [Survival System](#survival-system)
8. [Game Objectives](#game-objectives)
9. [Implementation Guidelines](#implementation-guidelines)

---

## Overview

The Zappy Server Protocol defines the communication interface between AI clients and the game server. This protocol enables autonomous players to navigate the world, collect resources, communicate with teammates, and progress through levels to achieve victory.

### Protocol Characteristics
- **Command-based interaction**: All actions performed through text commands
- **Asynchronous execution**: Commands queued and executed with realistic timing
- **Real-time survival**: Continuous food consumption requires active management
- **Team coordination**: Broadcast communication and collaborative leveling

---

## Connection Protocol

### Authentication Sequence

AI clients must complete the following handshake to join the game:

```
Server → Client: WELCOME
Client → Server: TEAM_NAME
Server → Client: CLIENT_NUM
Server → Client: X Y
```

**Connection Flow:**
1. Server sends welcome message upon client connection
2. Client responds with desired team name
3. Server assigns client number (0 indicates no slots available)
4. Server provides world dimensions if connection successful

### Connection States

| Client Number | Status | Action |
|---------------|--------|--------|
| 0 | No slots available | Connection automatically closed |
| 1-N | Valid slot assigned | Game participation enabled |

### Team Management
- Multiple clients can join the same team
- Team slots are limited by server configuration
- Clients on same team can coordinate for leveling

---

## Command System

### Command Queue Architecture

The server implements a sophisticated command queuing system:

- **Queue Capacity**: Maximum 10 pending commands per client
- **Execution Order**: First-In-First-Out (FIFO) processing
- **Blocking Behavior**: Commands execute sequentially with realistic timing
- **Overflow Handling**: Additional commands rejected until queue space available

### Timing Mechanism

All commands have execution times calculated using:

```
execution_time = base_time / frequency
```

**Default Configuration:**
- Base frequency: 100 time units
- Individual command base times vary by complexity
- Server administrators can modify frequency

### Response Format

| Response Type | Format | Description |
|---------------|--------|-------------|
| Success | `ok` | Command executed successfully |
| Failure | `ko` | Command failed or invalid |
| Death | `dead` | Player died, connection closing |

---

## Player Commands

### Movement Commands

#### Forward Movement
**Purpose:** Move one tile in current facing direction  
**Command:** `Forward`  
**Response:** `ok`  
**Execution Time:** `7/f`

**Behavior:**
- Moves to adjacent tile in facing direction
- Wraps around world boundaries
- Always succeeds (no terrain blocking)

#### Rotation Commands
**Turn Right**  
**Command:** `Right`  
**Response:** `ok`  
**Execution Time:** `7/f`

**Turn Left**  
**Command:** `Left`  
**Response:** `ok`  
**Execution Time:** `7/f`

**Orientation System:**
- 90-degree increments only
- Consistent with GUI orientation values
- No diagonal movement supported

### Information Commands

#### Environmental Scanning
**Command:** `Look`  
**Response:** `[tile0, tile1, tile2, ...]`  
**Execution Time:** `7/f`

Returns tile contents in vision pattern (see [Vision System](#vision-system)).

**Tile Content Format:**
Each tile contains space-separated list of objects:
- `food` `linemate` `deraumere` `sibur` `mendiane` `phiras` `thystame`
- `player` (other players on same tile)

#### Inventory Management
**Command:** `Inventory`  
**Response:** `[food n, linemate n, deraumere n, sibur n, mendiane n, phiras n, thystame n]`  
**Execution Time:** `1/f`

Returns current resource counts in fixed order.

#### Team Status
**Command:** `Connect_nbr`  
**Response:** `value`  
**Execution Time:** `0` (immediate)

Returns number of available team connection slots.

### Resource Commands

#### Resource Collection
**Command:** `Take OBJECT`  
**Response:** `ok` / `ko`  
**Execution Time:** `7/f`

**Valid Objects:**
- `food` `linemate` `deraumere` `sibur` `mendiane` `phiras` `thystame`

**Success Conditions:**
- Object exists on current tile
- Player has inventory space

#### Resource Dropping
**Command:** `Set OBJECT`  
**Response:** `ok` / `ko`  
**Execution Time:** `7/f`

**Success Conditions:**
- Player possesses the specified object
- Object successfully placed on current tile

### Communication Commands

#### Broadcasting Messages
**Command:** `Broadcast MESSAGE`  
**Response:** `ok`  
**Execution Time:** `7/f`

Sends message to all team members with directional information.

#### Receiving Broadcasts
**Format:** `message K`

| Parameter | Type | Description |
|-----------|------|-------------|
| message | String | Broadcast content |
| K | Integer | Direction indicator (0-8) |

**Direction Map:**
```
8 1 2
7 0 3
6 5 4
```
- 0: Same tile as sender
- 1-8: Relative direction to sender

### Advanced Commands

#### Reproduction
**Command:** `Fork`  
**Response:** `ok`  
**Execution Time:** `42/f`

Creates new egg for team member spawning.

**Mechanics:**
- Egg placed on current tile
- New team member will spawn from egg
- Increases team population capacity

#### Player Ejection
**Command:** `Eject`  
**Response:** `ok` / `ko`  
**Execution Time:** `7/f`

Forces other players off current tile.

**Success Conditions:**
- Other players present on tile
- Ejected players move to random adjacent tiles

#### Level Advancement
**Command:** `Incantation`  
**Response:** `Elevation underway` followed by `Current level: k` OR `ko`  
**Execution Time:** `300/f`

Attempts to advance player level through ritual incantation.

---

## Game Mechanics

### Incantation Requirements

Level advancement requires specific player counts and resource combinations:

#### Level 1 → 2
- **Required Players:** 1
- **Required Resources:** 1 linemate

#### Level 2 → 3
- **Required Players:** 2  
- **Required Resources:** 1 linemate, 1 deraumere, 1 sibur

#### Level 3 → 4
- **Required Players:** 2  
- **Required Resources:** 2 linemate, 1 deraumere, 2 sibur

#### Level 4 → 5
- **Required Players:** 4  
- **Required Resources:** 1 linemate, 1 deraumere, 2 sibur, 1 phiras

#### Level 5 → 6
- **Required Players:** 4  
- **Required Resources:** 1 linemate, 2 deraumere, 1 sibur, 1 mendiane

#### Level 6 → 7
- **Required Players:** 6  
- **Required Resources:** 1 linemate, 2 deraumere, 3 sibur, 1 phiras

#### Level 7 → 8
- **Required Players:** 6  
- **Required Resources:** 2 linemate, 2 deraumere, 2 sibur, 2 mendiane, 2 phiras, 1 thystame

### Incantation Process
1. All required players must be on same tile
2. Required resources must be present on tile
3. One player initiates incantation
4. All participating players advance to next level
5. Resources consumed during successful incantation

---

## Vision System

### Look Command Pattern

The `Look` command returns tiles in expanding vision pattern:

```
Level 1:     0

Level 2+:      0
             1 2 3
            4 5 6 7
         8 9 10 11 12
```

### Vision Mechanics

| Player Level | Visible Tiles | Pattern |
|--------------|---------------|---------|
| 1 | 1 | Current tile only |
| 2+ | 13 | Full diamond pattern |

### Orientation Dependency
- Pattern rotates based on player facing direction
- Tile 0 always represents current position
- Higher-numbered tiles extend further from player

### Tile Content Interpretation
Each tile in response contains:
- **Resources:** Object names separated by spaces
- **Players:** `player` indicator for each player present
- **Empty tiles:** Empty string or space

---

## Survival System

### Food Consumption Mechanics

**Consumption Rate:** 1 food unit every `126/f` time units

**Starvation Process:**
1. Food inventory reaches 0
2. Server sends `dead` message
3. Connection automatically closed
4. Player removed from game world

### Survival Strategies
- Maintain minimum food reserves
- Prioritize food collection over other resources
- Monitor consumption rate vs collection rate
- Use team coordination for food sharing areas

---

## Game Objectives

### Victory Conditions

**Primary Objective:** First team to achieve 6 players at maximum level (8)

**End Game Sequence:**
1. Victory condition met
2. Server broadcasts winner announcement
3. Game state frozen
4. All connections maintained for observation

### Strategic Considerations
- Balance individual advancement with team coordination
- Manage resource distribution across team members
- Coordinate incantation timing with resource availability
- Plan egg placement for optimal team expansion

---

## Implementation Guidelines

### Command Queue Management

**Optimal Practices:**
- Keep queue 70-80% full for maximum efficiency
- Reserve queue space for emergency commands
- Prioritize food collection commands during low reserves
- Batch movement commands for exploration

### Network Resilience

**Connection Handling:**
- Implement automatic reconnection logic
- Handle partial message reception
- Process server responses in order
- Maintain local state synchronization

### Resource Management

**Inventory Strategy:**
- Track resource collection rates
- Plan incantation resource requirements
- Coordinate with team for resource sharing
- Balance exploration vs collection activities

### Performance Optimization

**Algorithmic Considerations:**
- Cache frequently accessed game state
- Minimize unnecessary Look commands
- Use efficient pathfinding algorithms
- Implement smart resource gathering patterns

### Error Recovery

**Robust Implementation:**
- Handle `ko` responses gracefully
- Implement timeout mechanisms for commands
- Validate all server responses
- Maintain comprehensive logging

### Team Coordination

**Communication Strategies:**
- Establish team communication protocols
- Use broadcast system for coordination
- Plan incantation gatherings efficiently
- Share resource location information
