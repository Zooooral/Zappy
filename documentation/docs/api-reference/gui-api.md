# GUI API Protocol

## Table of Contents
1. [Overview](#overview)
2. [Connection Protocol](#connection-protocol)
3. [Request Commands](#request-commands)
4. [Event Notifications](#event-notifications)
5. [Error Handling](#error-handling)
6. [Reference Tables](#reference-tables)
7. [Implementation Guidelines](#implementation-guidelines)

---

## Overview

The Zappy GUI Protocol enables real-time visualization of the game world through a standardized communication interface between the server and graphical clients. This protocol provides comprehensive access to game state information and real-time event notifications.

### Protocol Characteristics
- **Read-only access**: GUI clients cannot modify game state
- **Multi-client support**: Multiple GUI instances can connect simultaneously
- **Real-time updates**: Automatic event broadcasting to all connected GUIs
- **Message format**: All messages are newline-terminated ASCII text

---

## Connection Protocol

### Authentication Sequence

The GUI client must authenticate with the server using the following handshake:

```
Server → GUI: WELCOME
GUI → Server: GRAPHIC
Server → GUI: X Y
```

**Description:**
1. Server sends welcome message upon client connection
2. GUI identifies itself by sending `GRAPHIC` instead of a team name
3. Server responds with world dimensions (X=width, Y=height)

### Connection State Management
- Connection is persistent until client disconnects
- No periodic heartbeat required
- Server handles client disconnection gracefully

---

## Request Commands

GUI clients can query the server for specific information using the following commands:

### World Information

#### Get Map Dimensions
**Purpose:** Retrieve the world size  
**Request:** `msz`  
**Response:** `msz X Y`

| Parameter | Type | Description |
|-----------|------|-------------|
| X | Integer | World width (0-based indexing) |
| Y | Integer | World height (0-based indexing) |

#### Get Complete Map State
**Purpose:** Request all tile contents  
**Request:** `mct`  
**Response:** Multiple `bct` responses (one per tile)

#### Get Specific Tile Content
**Purpose:** Query resources on a specific tile  
**Request:** `bct X Y`  
**Response:** `bct X Y q0 q1 q2 q3 q4 q5 q6`

| Parameter | Type | Description |
|-----------|------|-------------|
| X | Integer | Tile X coordinate |
| Y | Integer | Tile Y coordinate |
| q0-q6 | Integer | Resource quantities (see [Resource Types](#resource-types)) |

### Team Management

#### Get Team List
**Purpose:** Retrieve all team names  
**Request:** `tna`  
**Response:** `tna TEAM_NAME` (one message per team)

### Player Information

#### Get Player Position
**Purpose:** Query specific player location and orientation  
**Request:** `ppo #N`  
**Response:** `ppo #N X Y O`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | Player identifier (prefixed with #) |
| X | Integer | Player X coordinate |
| Y | Integer | Player Y coordinate |
| O | Integer | Player orientation (see [Orientations](#orientations)) |

#### Get Player Level
**Purpose:** Query player's current level  
**Request:** `plv #N`  
**Response:** `plv #N L`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | Player identifier |
| L | Integer | Current player level (1-8) |

#### Get Player Inventory
**Purpose:** Query player's resource inventory  
**Request:** `pin #N`  
**Response:** `pin #N X Y q0 q1 q2 q3 q4 q5 q6`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | Player identifier |
| X | Integer | Player X coordinate |
| Y | Integer | Player Y coordinate |
| q0-q6 | Integer | Resource quantities in inventory |

### Server Configuration

#### Get Time Unit
**Purpose:** Query current server time unit  
**Request:** `sgt`  
**Response:** `sgt T`

| Parameter | Type | Description |
|-----------|------|-------------|
| T | Integer | Current time unit value |

#### Set Time Unit
**Purpose:** Modify server time unit  
**Request:** `sst T`  
**Response:** `sst T`

| Parameter | Type | Description |
|-----------|------|-------------|
| T | Integer | New time unit value |

---

## Event Notifications

The server automatically broadcasts events to all connected GUI clients when game state changes occur.

### Player Lifecycle Events

#### New Player Connection
**Event:** `pnw #N X Y O L TEAM_NAME`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | New player identifier |
| X | Integer | Starting X coordinate |
| Y | Integer | Starting Y coordinate |
| O | Integer | Initial orientation |
| L | Integer | Starting level |
| TEAM_NAME | String | Player's team name |

#### Player Movement
**Event:** `ppo #N X Y O`

Updates player position and orientation in real-time.

#### Player Level Change
**Event:** `plv #N L`

Notifies when a player successfully levels up.

#### Player Inventory Update
**Event:** `pin #N X Y q0 q1 q2 q3 q4 q5 q6`

Broadcasts inventory changes after resource collection or consumption.

#### Player Death
**Event:** `pdi #N`

Indicates player has died and been removed from the game.

#### Player Expulsion
**Event:** `pex #N`

Notifies when a player has been expelled from a tile.

### Action Events

#### Broadcast Message
**Event:** `pbc #N MESSAGE`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | Message sender |
| MESSAGE | String | Broadcast content |

#### Resource Manipulation
**Drop Resource:** `pdr #N i`  
**Collect Resource:** `pgt #N i`

| Parameter | Type | Description |
|-----------|------|-------------|
| #N | Player ID | Player performing action |
| i | Integer | Resource type identifier |

#### Egg Management
**Egg Laying Started:** `pfk #N`  
**Egg Laid:** `enw #e #N X Y`  
**Egg Hatching:** `ebo #e`  
**Player from Egg:** `edi #e`

| Parameter | Type | Description |
|-----------|------|-------------|
| #e | Egg ID | Egg identifier |
| #N | Player ID | Parent player |

### Incantation Events

#### Incantation Start
**Event:** `pic X Y L #n1 #n2 ...`

| Parameter | Type | Description |
|-----------|------|-------------|
| X | Integer | Incantation X coordinate |
| Y | Integer | Incantation Y coordinate |
| L | Integer | Required level for incantation |
| #n1, #n2... | Player IDs | Participating players |

#### Incantation End
**Event:** `pie X Y R`

| Parameter | Type | Description |
|-----------|------|-------------|
| X | Integer | Incantation X coordinate |
| Y | Integer | Incantation Y coordinate |
| R | Integer | Result (0=failure, 1=success) |

### Game State Events

#### Game End
**Event:** `seg TEAM_NAME`

Indicates winning team and game conclusion.

#### Server Message
**Event:** `smg MESSAGE`

Administrative messages from the server.

---

## Error Handling

### Error Response Codes

| Response | Description | Common Causes |
|----------|-------------|---------------|
| `suc` | Unknown Command | Invalid command syntax |
| `sbp` | Bad Parameters | Incorrect parameter count or format |

### Best Practices
- Always validate responses before processing
- Implement timeout mechanisms for requests
- Handle network disconnections gracefully
- Buffer incoming messages properly

---

## Reference Tables

### Resource Types

| ID | Name | Description |
|----|------|-------------|
| 0 | food | Basic sustenance resource |
| 1 | linemate | Level 1 incantation stone |
| 2 | deraumere | Level 2 incantation stone |
| 3 | sibur | Level 3 incantation stone |
| 4 | mendiane | Level 4 incantation stone |
| 5 | phiras | Level 5 incantation stone |
| 6 | thystame | Level 6 incantation stone |

### Orientations

| Value | Direction | Vector |
|-------|-----------|---------|
| 1 | North | (0, -1) |
| 2 | East | (1, 0) |
| 3 | South | (0, 1) |
| 4 | West | (-1, 0) |

### Player Levels

| Level | Max Players | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
|-------|-------------|----------|-----------|-------|----------|---------|----------|
| 1→2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2→3 | 2 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3→4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4→5 | 4 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5→6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6→7 | 6 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7→8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

---

## Implementation Guidelines

### Network Considerations
- Use non-blocking socket operations
- Implement proper message buffering
- Handle partial message reception
- Process messages in order of arrival

### Performance Optimization
- Cache frequently requested data
- Batch process multiple events
- Minimize unnecessary server queries
- Use efficient data structures for game state

### Error Recovery
- Reconnect automatically on connection loss
- Resynchronize game state after reconnection
- Validate all received data
- Implement logging for debugging

### GUI Integration
- Update display asynchronously from network events
- Maintain local game state cache
- Handle concurrent updates properly
- Provide visual feedback for network status
