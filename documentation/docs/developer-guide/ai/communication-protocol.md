# AI Client Protocol

## Table of Contents
1. [Overview](#overview)
2. [Connection Protocol](#connection-protocol)
3. [Command Reference](#command-reference)
4. [Server Messages](#server-messages)
5. [Technical Implementation](#technical-implementation)
6. [Implementation Guidelines](#implementation-guidelines)

---

## Overview

The Zappy AI Client Protocol defines the communication interface between autonomous AI clients and the game server. AI clients participate actively in the game, executing commands and receiving real-time feedback about the game world.

### Protocol Characteristics
- **Active participation**: AI clients can modify game state through commands
- **Asynchronous execution**: Commands are queued and executed with timing constraints
- **Real-time feedback**: Server broadcasts events and command responses
- **Message format**: All messages are newline-terminated ASCII text
- **Language agnostic**: Protocol can be implemented in any language supporting TCP sockets

---

## Connection Protocol

### Authentication Sequence

AI clients must authenticate with the server using the following handshake:

```
Server → Client: WELCOME
Client → Server: TEAM_NAME
Server → Client: CLIENT_NUM
Server → Client: X Y
```

**Description:**
1. Server sends welcome message upon client connection
2. Client identifies itself by sending its team name
3. Server responds with the number of available slots for this team
4. Server sends world dimensions (X=width, Y=height)

### Connection Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| TEAM_NAME | String | Team identifier (must match server configuration) |
| CLIENT_NUM | Integer | Available connection slots for the team |
| X | Integer | World width |
| Y | Integer | World height |

### Command Queue Management
- **Queue Limit**: Clients can send up to 10 commands before receiving responses
- **Execution Order**: Commands are processed in FIFO order
- **Blocking Behavior**: Only the executing player is blocked during command execution
- **Overflow Handling**: Commands beyond the 10-command limit are ignored

---

## Command Reference

All commands are executed with timing constraints based on the server's time unit configuration.

### Movement Commands

#### Forward
**Purpose:** Move forward one tile in current direction  
**Command:** `Forward`  
**Execution Time:** 7 time units  
**Response:** `ok`  

#### Turn Right
**Purpose:** Rotate orientation 90° clockwise  
**Command:** `Right`  
**Execution Time:** 7 time units  
**Response:** `ok`  

#### Turn Left
**Purpose:** Rotate orientation 90° counter-clockwise  
**Command:** `Left`  
**Execution Time:** 7 time units  
**Response:** `ok`  

### Information Commands

#### Look Around
**Purpose:** Get vision of surrounding tiles  
**Command:** `Look`  
**Execution Time:** 7 time units  
**Response:** `[tile1, tile2, ...]`  

**Vision Format:**
- Returns array of tiles in vision range (level-dependent)
- Each tile contains comma-separated resource list
- Resources: `player`, `food`, `linemate`, `deraumere`, `sibur`, `mendiane`, `phiras`, `thystame`
- Vision follows specific geometric pattern based on player level

#### Inventory Check
**Purpose:** Query current inventory contents  
**Command:** `Inventory`  
**Execution Time:** 1 time unit  
**Response:** `[food n, linemate n, deraumere n, sibur n, mendiane n, phiras n, thystame n]`  

### Resource Management

#### Take Object
**Purpose:** Collect resource from current tile  
**Command:** `Take RESOURCE`  
**Execution Time:** 7 time units  
**Response:** `ok` (success) or `ko` (failure)  

**Valid Resources:**
- `food`
- `linemate`
- `deraumere`
- `sibur`
- `mendiane`
- `phiras`
- `thystame`

#### Set Object Down
**Purpose:** Drop resource on current tile  
**Command:** `Set RESOURCE`  
**Execution Time:** 7 time units  
**Response:** `ok` (success) or `ko` (failure)  

### Communication Commands

#### Broadcast Message
**Purpose:** Send message to all team members  
**Command:** `Broadcast MESSAGE`  
**Execution Time:** 7 time units  
**Response:** `ok`  

**Message Reception:**
Other players receive broadcasts as: `message DIRECTION, MESSAGE`
- DIRECTION: Integer 1-8 indicating source direction relative to receiver

#### Connect Number
**Purpose:** Query available team connection slots  
**Command:** `Connect_nbr`  
**Execution Time:** 0 time units  
**Response:** `VALUE` (integer representing available slots)  

### Lifecycle Commands

#### Fork Player
**Purpose:** Create new connection slot for team  
**Command:** `Fork`  
**Execution Time:** 42 time units  
**Response:** `ok`  

#### Eject Players
**Purpose:** Push other players from current tile  
**Command:** `Eject`  
**Execution Time:** 7 time units  
**Response:** `ok` (success) or `ko` (no players to eject)  

**Ejection Effects:**
- Ejected players receive: `eject DIRECTION`
- Ejected players move one tile in ejection direction

#### Incantation
**Purpose:** Attempt level elevation  
**Command:** `Incantation`  
**Execution Time:** 300 time units  
**Response:** `Elevation underway\nCurrent level: K` (success) or `ko` (failure)  

**Requirements:**
- Sufficient players of same level on tile
- Required resources present on tile
- See [Elevation Requirements](#elevation-requirements) for details

---

## Server Messages

### Real-time Events

AI clients receive asynchronous messages for real-time game events:

#### Broadcast Reception
**Message:** `message DIRECTION, MESSAGE`  
Received when another player broadcasts a message.

#### Death Notification
**Message:** `dead`  
Indicates the player has died (starvation or other causes).

#### Ejection Notification
**Message:** `eject DIRECTION`  
Received when ejected from a tile by another player.

#### Incantation Events
**Start Message:** `Elevation underway`  
**Completion:** `Current level: K` (where K is the new level)

### Error Responses
| Response | Description | Cause |
|----------|-------------|-------|
| `ko` | Command failed | Invalid command parameters or game state |
| `dead` | Player died | Starvation or other fatal condition |

---

## Technical Implementation

### Network Protocol
- **Transport:** TCP sockets
- **Port:** Configurable (specified in server arguments)
- **Encoding:** ASCII text
- **Termination:** All messages end with newline (`\n`)

### Time Unit System
- **Base Unit:** Configurable server parameter (default: reciprocal of frequency)
- **Calculation:** `execution_time = action_duration / frequency`
- **Example:** With frequency=100, Forward command takes 7/100 = 0.07 seconds

### Action Duration Table
| Command | Time Units |
|---------|------------|
| Forward | 7 |
| Right | 7 |
| Left | 7 |
| Look | 7 |
| Inventory | 1 |
| Broadcast | 7 |
| Connect_nbr | 0 |
| Fork | 42 |
| Eject | 7 |
| Take | 7 |
| Set | 7 |
| Incantation | 300 |

### Elevation Requirements

| Level Transition | Players Required | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
|------------------|------------------|----------|-----------|-------|----------|---------|----------|
| 1 → 2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2 → 3 | 2 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3 → 4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4 → 5 | 4 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5 → 6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6 → 7 | 6 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7 → 8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

---

## Implementation Guidelines

### Connection Management
- Implement robust connection handling with automatic reconnection
- Handle server disconnections gracefully
- Validate authentication responses before proceeding

### Command Queue Management
```typescript
class CommandQueue {
    private pendingCommands: Command[] = [];
    private readonly MAX_PENDING = 10;
    
    async sendCommand(command: string): Promise<string> {
        if (this.pendingCommands.length >= this.MAX_PENDING) {
            throw new Error("Command queue full");
        }
        // Implementation details...
    }
}
```

### Message Parsing
- Implement proper message buffering for partial receives
- Handle multiple messages in single network packet
- Parse server messages asynchronously from command responses

### Error Handling
- Distinguish between command failures (`ko`) and fatal errors (`dead`)
- Implement timeout mechanisms for long-running commands
- Handle network interruptions and protocol errors

### Strategy Implementation
- Maintain internal world state model
- Implement goal-based decision making
- Use pathfinding algorithms for efficient movement
- Coordinate with team members through broadcasts

### Performance Considerations
- Cache frequently accessed game state
- Minimize unnecessary Look commands
- Batch resource collection when possible
- Optimize communication patterns

### Example Implementation Structure
```typescript
class ZappyAIClient extends EventEmitter {
    private connection: NetworkConnection;
    private worldModel: WorldModel;
    private strategy: StrategyEngine;
    
    async connect(host: string, port: number, teamName: string): Promise<void> {
        // Connection implementation
    }
    
    async executeCommand(command: string): Promise<string> {
        // Command execution with queue management
    }
    
    private handleServerMessage(message: string): void {
        // Asynchronous message handling
    }
}
```

### Testing and Debugging
- Implement comprehensive logging for all protocol interactions
- Use mock servers for unit testing strategy components
- Monitor command timing and queue utilization
- Test error recovery scenarios