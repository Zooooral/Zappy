# Server Architecture

## Table of Contents
1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Core Components](#core-components)
4. [Data Flow](#data-flow)
5. [Module Organization](#module-organization)
6. [Lifecycle Management](#lifecycle-management)
7. [Configuration System](#configuration-system)
8. [Development Guidelines](#development-guidelines)

---

## Overview

The Zappy server is a high-performance, event-driven C application that manages a multiplayer game simulation. It implements a modular architecture with clear separation between networking, game logic, resource management, and protocol handling.

### Key Design Principles
- **Single-threaded event-driven model**: Uses `poll()` for efficient I/O multiplexing
- **Modular architecture**: Clear separation of concerns across functional modules
- **Memory efficiency**: Dynamic allocation with careful resource management
- **Protocol abstraction**: Separate handlers for AI and GUI client protocols
- **Time-based simulation**: Configurable game timing with action queuing

---

## System Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Zappy Server                        │
├─────────────────┬─────────────────┬─────────────────────────┤
│   Network Layer │   Game Engine   │    Resource Manager     │
│                 │                 │                         │
│ • Socket Mgmt   │ • Game State    │ • Map Management        │
│ • Client Mgmt   │ • Player Mgmt   │ • Resource Spawning     │
│ • Protocol Impl │ • Time System   │ • Tile Operations       │
│ • Event Loop    │ • Action Queue  │ • Geography Rules       │
└─────────────────┴─────────────────┴─────────────────────────┘
                            │
                ┌───────────┼───────────┐
                │           │           │
        ┌───────▼───┐ ┌─────▼─────┐ ┌───▼──────┐
        │ AI Client │ │ AI Client │ │GUI Client│
        │  Team A   │ │  Team B   │ │ Observer │
        └───────────┘ └───────────┘ └──────────┘
```

### Component Interaction

| Component | Responsibilities | Dependencies |
|-----------|-----------------|--------------|
| **Network Layer** | Connection handling, message processing | Signal handling, poll management |
| **Game Engine** | Game state, player management, timing | Map system, resource management |
| **Protocol Layer** | Command parsing, response formatting | Game engine, client management |
| **Resource Manager** | Map operations, resource spawning | Game state, random generation |

---

## Core Components

### Server Structure

The main server structure centralizes all system state:

```c
typedef struct server_s {
    int server_fd;                    // Main server socket
    int signal_fd;                    // Signal handling
    server_config_t config;           // Configuration parameters
    client_t *clients;                // Connected clients array
    size_t client_count;              // Active client count
    struct pollfd *poll_fds;          // Poll file descriptors
    bool is_running;                  // Server state flag
    game_state_t *game;               // Game world state
    size_t tick_count;                // Game tick counter
} server_t;
```

### Game State Management

```c
typedef struct game_state_s {
    map_t *map;                       // Game world map
    player_t **players;               // Active players array
    size_t player_count;              // Number of players
    double current_time;              // Game simulation time
    int next_player_id;               // Player ID generator
} game_state_t;
```

### Map System

The world is represented as a torus with discrete tiles:

```c
typedef struct tile_s {
    int resources[RESOURCE_COUNT];    // Resource quantities per tile
    player_t **players;               // Players on this tile
    size_t player_count;              // Number of players on tile
} tile_t;

typedef struct map_s {
    tile_t **tiles;                   // 2D tile array
    int width;                        // Map width
    int height;                       // Map height
} map_t;
```

### Player Management

Players represent AI clients in the game world:

```c
typedef struct player_s {
    int id;                           // Unique player identifier
    int x, y;                         // Position coordinates
    int orientation;                  // Facing direction (1-4)
    int level;                        // Current player level
    char *team_name;                  // Team affiliation
    int resources[RESOURCE_COUNT];    // Player inventory
    bool is_alive;                    // Survival status
    client_t *client;                 // Associated network client
} player_t;
```

---

## Data Flow

### Message Processing Flow

```
Client Message → Buffer → Parse → Protocol Handler → Game Logic → Response
     ↓              ↓        ↓           ↓              ↓           ↓
Network Layer → Buffering → Parsing → Dispatch → State Update → Broadcast
```

### Game Update Cycle

```
Poll Events → Handle Network → Process Actions → Update Game → Check Win → Broadcast
     ↓              ↓              ↓              ↓           ↓           ↓
I/O Events → Client Messages → Timed Commands → World State → Victory → GUI Updates
```

### Resource Management Flow

```
Resource Spawn → Map Placement → Player Interaction → Collection → Inventory Update
      ↓               ↓               ↓               ↓              ↓
 Random Gen → Tile Assignment → Command Processing → State Change → Broadcast
```

---

## Module Organization

### Directory Structure

```
src/server/
├── args/                    # Command line argument parsing
│   ├── argument_parser.c    # Main parsing logic
│   └── argument_handler.c   # Individual argument processors
├── client/                  # Client connection management
│   ├── client_management.c  # Client lifecycle
│   └── client_helper.c      # Message processing utilities
├── game/                    # Game logic implementation
│   ├── game_state.c         # Game state management
│   ├── player.c             # Player operations
│   └── map.c                # Map and tile operations
├── network/                 # Network layer implementation
│   ├── network_handler.c    # Event loop and I/O handling
│   └── payload_*.c          # Message formatting utilities
├── protocol/                # Protocol implementations
│   ├── protocol_ai.c        # AI client protocol
│   ├── protocol_graphic.c   # GUI client protocol
│   └── ai_commands.c        # Command dispatch tables
├── server_init.c            # Server initialization
├── server_run.c             # Main event loop
├── server_cleanup.c         # Resource cleanup
├── signal_handler.c         # Signal management
├── time.c                   # Time and action queue system
└── resource.c               # Resource management
```

### Header Organization

```
include/server/
├── server.h                 # Main server structures and declarations
├── client_management.h      # Client operations
├── protocol_ai.h            # AI protocol definitions
├── protocol_graphic.h       # GUI protocol definitions
├── time.h                   # Time and action system
├── resource.h               # Resource management
└── signal_handler.h         # Signal handling
```

---

## Lifecycle Management

### Server Initialization Sequence

1. **Argument Parsing**: Process command line parameters and validate configuration
2. **Signal Setup**: Initialize signal handling for graceful shutdown
3. **Socket Creation**: Create and configure server socket with appropriate options
4. **Memory Allocation**: Allocate client arrays and poll file descriptor arrays
5. **Game World Creation**: Initialize map, spawn initial resources
6. **Event Loop Setup**: Configure poll structures and enter main loop

### Client Connection Lifecycle

1. **Connection Accept**: Accept new client socket connection
2. **Client Registration**: Add to client array and poll structures
3. **Authentication**: Process team name or GUI identification
4. **Player Creation**: For AI clients, create associated player object
5. **Active Phase**: Process commands and maintain game state
6. **Disconnection**: Clean up resources and remove from active structures

### Game State Evolution

1. **Initialization**: Create empty world map with resource distribution
2. **Player Spawning**: Place new players at random valid positions
3. **Command Processing**: Execute player actions through action queue system
4. **World Updates**: Update resource spawning, player food consumption
5. **Victory Conditions**: Monitor for team elevation completion
6. **Cleanup**: Proper resource deallocation on game end

---

## Configuration System

### Command Line Arguments

The server accepts the following configuration parameters:

| Argument | Description | Validation |
|----------|-------------|------------|
| `-p port` | Server port number | Range: 1024-65535 |
| `-x width` | Map width | Range: 6-50 |
| `-y height` | Map height | Range: 6-50 |
| `-n team1 team2 ...` | Team names | At least one team |
| `-c clients` | Max clients per team | Positive integer |
| `-f freq` | Time unit frequency | Positive integer |

### Configuration Validation

```c
static int validate_config(const server_config_t *config)
{
    if (config->port < 1024 || config->port > 65535) return -1;
    if (config->width < 6 || config->width > 50) return -1;
    if (config->height < 6 || config->height > 50) return -1;
    if (config->team_count == 0) return -1;
    if (config->max_clients_per_team == 0) return -1;
    if (config->freq == 0) return -1;
    return 0;
}
```

### Runtime Configuration

- **Time Unit**: Configurable game speed via frequency parameter
- **Resource Density**: Calculated based on map size using predefined formulas
- **Player Limits**: Per-team client limits enforced during authentication
- **Map Properties**: Torus topology with wrapping coordinates

---

## Development Guidelines

### Adding New Features

#### Game Logic Extensions

1. **State Management**: Add new fields to appropriate structures in `server.h`
2. **Initialization**: Update creation functions to initialize new state
3. **Update Logic**: Modify game update cycle to handle new mechanics
4. **Protocol Integration**: Add new commands or responses as needed
5. **Cleanup**: Ensure proper resource deallocation

#### Protocol Extensions

1. **Command Definition**: Add entries to command dispatch tables
2. **Handler Implementation**: Create command-specific handler functions
3. **Validation**: Implement parameter validation and error handling
4. **Response Formatting**: Follow existing message format conventions
5. **Testing**: Verify protocol compliance and error cases

### Performance Considerations

#### Memory Management
- **Dynamic Arrays**: Use realloc for growing arrays with doubling strategy
- **String Handling**: Prefer stack allocation for temporary strings
- **Resource Cleanup**: Always pair allocation with deallocation
- **Leak Prevention**: Use tools like Valgrind for memory profiling

#### Event Loop Optimization
- **Poll Efficiency**: Minimize poll array modifications
- **Message Batching**: Process multiple messages per poll cycle
- **Non-blocking Operations**: Avoid blocking on individual client operations
- **Resource Limits**: Enforce limits to prevent resource exhaustion

### Code Quality Standards

#### Naming Conventions
- **Functions**: `module_action` format (e.g., `client_add`, `player_create`)
- **Structures**: `name_t` suffix for type definitions
- **Constants**: `UPPER_CASE` with descriptive prefixes
- **Variables**: `snake_case` with meaningful names

#### Error Handling
- **Return Values**: Use -1 for errors, 0 for success
- **Parameter Validation**: Check all pointer parameters for NULL
- **Resource Cleanup**: Handle partial initialization failures
- **Error Propagation**: Propagate errors through call stack

This architecture documentation provides developers with the essential understanding needed to work with the Zappy server codebase, focusing on the overall design patterns and key implementation decisions that shape the system.