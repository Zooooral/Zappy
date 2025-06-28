# Zappy Server Network Documentation

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Core Data Structures](#core-data-structures)
4. [Socket Management](#socket-management)
5. [Client Connection Handling](#client-connection-handling)
6. [Message Processing](#message-processing)
7. [Protocol Implementation](#protocol-implementation)
8. [Event Loop](#event-loop)
9. [Action Queue System](#action-queue-system)
10. [Error Handling](#error-handling)
11. [Performance Considerations](#performance-considerations)
12. [Implementation Guidelines](#implementation-guidelines)

---

## Overview

The Zappy server network layer implements a high-performance, event-driven TCP server capable of handling multiple concurrent client connections. The architecture supports two distinct client types: AI clients that participate in the game and GUI clients that observe game state in real-time.

### Key Features
- **Event-driven architecture**: Uses `poll()` system call for efficient I/O multiplexing
- **Non-blocking socket operations**: Prevents server blocking on individual client operations
- **Dual protocol support**: Separate protocol handlers for AI and GUI clients
- **Action queue system**: Time-based command execution for AI clients
- **Memory-efficient buffering**: Dynamic buffer management for client message processing
- **Signal handling**: Graceful shutdown on system signals

---

## Architecture

### Network Layer Components

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Server Init   │───▶│   Event Loop     │───▶│   Cleanup       │
│                 │    │                  │    │                 │
│ • Socket Setup  │    │ • Poll Events    │    │ • Close Sockets │
│ • Client Arrays │    │ • Accept Conns   │    │ • Free Memory   │
│ • Poll Setup    │    │ • Process Msgs   │    │ • Signal Handler│
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
        ┌───────▼─────┐ ┌───────▼─────┐ ┌───────▼─────┐
        │   Client    │ │  Protocol   │ │   Action    │
        │ Management  │ │  Handling   │ │   Queue     │
        │             │ │             │ │             │
        │ • Add/Remove│ │ • AI Cmds   │ │ • Timed Exec│
        │ • Find by FD│ │ • GUI Cmds  │ │ • FIFO Order│
        │ • Auth      │ │ • Dispatch  │ │ • Callbacks │
        └─────────────┘ └─────────────┘ └─────────────┘
```

### File Organization

The network implementation is organized across multiple files:

| File | Purpose |
|------|---------|
| `server_init.c` | Server initialization and socket setup |
| `server_run.c` | Main event loop and game state updates |
| `server_cleanup.c` | Resource cleanup and shutdown procedures |
| `network_handler.c` | Network event processing and connection handling |
| `client_management.c` | Client lifecycle and connection management |
| `client_helper.c` | Message buffering and processing utilities |
| `protocol_ai.c` | AI client protocol implementation |
| `protocol_graphic.c` | GUI client protocol implementation |
| `time.c` | Action queue and time management |

---

## Core Data Structures

### Server Structure

```c
typedef struct server_s {
    int server_fd;                    // Main server socket file descriptor
    int signal_fd;                    // Signal handling file descriptor
    server_config_t config;           // Server configuration parameters
    client_t *clients;                // Dynamic array of client connections
    size_t client_count;              // Current number of connected clients
    size_t client_capacity;           // Maximum client capacity
    struct pollfd *poll_fds;          // Poll file descriptor array
    size_t poll_count;                // Number of file descriptors to poll
    bool is_running;                  // Server running state flag
    game_state_t *game;               // Game state management
    size_t tick_count;                // Server tick counter
} server_t;
```

### Client Structure

```c
typedef struct client_s {
    int fd;                           // Client socket file descriptor
    client_type_t type;               // CLIENT_TYPE_AI or CLIENT_TYPE_GRAPHIC
    char *buffer;                     // Dynamic message buffer
    size_t buffer_size;               // Current buffer allocated size
    size_t buffer_pos;                // Current position in buffer
    bool is_authenticated;            // Authentication status
    char *team_name;                  // Team name for AI clients
    action_t *action_queue_head;      // Head of action queue
    action_t *action_queue_tail;      // Tail of action queue
    size_t action_queue_count;        // Number of queued actions
    struct player_s *player;          // Associated player object
} client_t;
```

### Action Structure

```c
typedef struct action_s {
    char *command;                    // Original command string
    double exec_time;                 // Scheduled execution time
    void (*callback)(client_t *client, void *data); // Execution callback
    void *data;                       // Action-specific data
    struct action_s *next;            // Next action in queue
} action_t;
```

---

## Socket Management

### Server Socket Initialization

The server socket initialization follows a structured approach:

#### Socket Creation and Configuration

```c
static int create_server_socket(size_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (fd == -1)
        return -1;
    if (setup_socket_options(fd) == -1)
        return -1;
    if (bind_socket_to_port(fd, port) == -1)
        return -1;
    if (listen(fd, 10) == -1) {
        close(fd);
        return -1;
    }
    return fd;
}
```

#### Socket Options Configuration

Critical socket options are configured for optimal performance:

- **SO_REUSEADDR**: Allows immediate reuse of the server port after shutdown
- **Listen backlog**: Set to 10 for adequate connection queuing

#### Address Binding

The server binds to all available interfaces (`INADDR_ANY`) on the specified port, allowing connections from any network interface.

---

## Client Connection Handling

### Connection Acceptance

New client connections are handled through the `accept()` system call:

```c
static void handle_new_connection(server_t *server)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server->server_fd,
        (struct sockaddr *)&client_addr, &addr_len);

    if (client_fd == -1) {
        printf("[SERVER] Accept failed: %s\n", strerror(errno));
        return;
    }
    if (client_add(server, client_fd) == -1) {
        printf("[SERVER] Failed to add client\n");
        close(client_fd);
    }
}
```

### Client Registration Process

#### Initial Setup

1. **Socket Registration**: New client socket is added to the poll file descriptor array
2. **Buffer Initialization**: Dynamic buffer allocated with initial size of 4096 bytes
3. **State Initialization**: Client state set to unauthenticated with unknown type
4. **Welcome Message**: "WELCOME\n" sent immediately upon connection

#### Authentication Sequence

Clients must authenticate before accessing game functionality:

```
Server → Client: WELCOME\n
Client → Server: TEAM_NAME\n (for AI) or GRAPHIC\n (for GUI)
Server → Client: CLIENT_NUM X Y\n (for AI) or X Y\n (for GUI)
```

### Client Removal Process

Client disconnection is handled gracefully:

1. **Resource Cleanup**: Buffers, team names, and action queues are freed
2. **Socket Closure**: Client socket file descriptor is closed
3. **Array Compaction**: Client array is shifted to maintain contiguous storage
4. **Poll Array Update**: Poll file descriptor array is synchronized
5. **Game State Update**: Associated player objects are removed from game state

---

## Message Processing

### Buffer Management

#### Dynamic Buffer Expansion

Client buffers expand automatically when receiving large messages:

```c
static int expand_client_buffer(client_t *client, ssize_t received)
{
    if (client->buffer_pos + received >= client->buffer_size) {
        client->buffer_size *= 2;
        client->buffer = realloc(client->buffer, client->buffer_size);
        if (!client->buffer)
            return -1;
    }
    return 0;
}
```

#### Message Boundary Detection

Messages are delimited by newline characters (`\n`). The server processes complete messages and maintains partial messages in the buffer:

```c
static void process_received_messages(server_t *server, client_t *client)
{
    char *newline = memchr(client->buffer, '\n', client->buffer_pos);

    while (newline) {
        process_message_line(server, client, newline);
        newline = memchr(client->buffer, '\n', client->buffer_pos);
    }
}
```

### Non-blocking I/O

All socket operations use non-blocking mode with proper error handling:

```c
ssize_t received = recv(client->fd, buffer, 1023, MSG_DONTWAIT);

if (received == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0;  // No data available, try again later
    return -1;     // Connection error
}
```

---

## Protocol Implementation

### AI Client Protocol

#### Command Dispatch Table

AI commands are processed through a dispatch table for efficient routing:

```c
const ai_cmd_t ai_commands[] = {
    {"Forward", 7, handle_forward},
    {"Right", 5, handle_right},
    {"Left", 4, handle_left},
    {"Look", 4, handle_look},
    {"Inventory", 9, handle_inventory},
    {"Broadcast ", 10, handle_broadcast},
    {"Connect_nbr", 11, handle_connect_nbr},
    {"Fork", 4, handle_fork},
    {"Eject", 5, handle_eject},
    {"Take ", 5, handle_take},
    {"Set ", 4, handle_set},
    {"Incantation", 11, handle_incantation},
};
```

#### Time-based Execution

AI commands have associated execution durations and are queued for delayed execution:

```c
static const double ai_action_duration[] = {
    [AI_ACTION_FORWARD] = 7.0,
    [AI_ACTION_RIGHT] = 7.0,
    [AI_ACTION_LEFT] = 7.0,
    [AI_ACTION_LOOK] = 7.0,
    [AI_ACTION_INVENTORY] = 1.0,
    [AI_ACTION_BROADCAST] = 7.0,
    [AI_ACTION_CONNECT_NBR] = 0.0,
    [AI_ACTION_FORK] = 42.0,
    [AI_ACTION_EJECT] = 7.0,
    [AI_ACTION_TAKE] = 7.0,
    [AI_ACTION_SET] = 7.0,
    [AI_ACTION_INCANTATION] = 300.0
};
```

### GUI Client Protocol

#### Command Table

GUI commands provide read-only access to game state:

```c
const graphic_cmd_entry_t graphic_commands[] = {
    {"msz", handle_map_size_command},
    {"mct", handle_map_content_command},
    {"tna", handle_team_names_command},
    {"bct ", handle_tile_content_command},
    {"pnw", handle_player_info_command},
    {"ppo", handle_position_update},
    {"pin", handle_player_inventory},
    {"sgt", handle_time_unit_command},
    {"sst", handle_time_unit_modification},
    {NULL, NULL}
};
```

#### Event Broadcasting

GUI clients receive automatic notifications for game state changes:

- Player connections and disconnections
- Player movement and orientation changes
- Resource collection and placement
- Incantation events
- Game end conditions

---

## Event Loop

### Main Loop Structure

The server operates on a single-threaded event loop using the `poll()` system call:

```c
void server_run(server_t *server)
{
    double last_time = get_current_time();
    double delta_time;

    print_server_info(server);
    while (server->is_running) {
        delta_time = calculate_delta_time(&last_time);
        
        if (check_for_shutdown_signal(server))
            break;
            
        if (handle_poll_events(server) == -1)
            break;
            
        wait_for_next_tick(server, delta_time);
    }
    printf("[SERVER] Server shutting down\n");
}
```

### Poll Event Handling

The event handling prioritizes different event types:

1. **Signal Events**: Check for shutdown signals (highest priority)
2. **New Connections**: Accept new client connections
3. **Client Data**: Process incoming client messages
4. **Game Updates**: Update game state and process action queues

### Error Event Handling

Client socket errors are detected through poll events:

```c
static bool should_remove_client(struct pollfd *pfd)
{
    if (pfd->revents & (POLLHUP | POLLERR | POLLNVAL)) {
        printf("[SERVER] Client fd:%d has error condition (revents: %d)\n",
            pfd->fd, pfd->revents);
        return true;
    }
    return false;
}
```

---

## Action Queue System

### Queue Management

Each AI client maintains a FIFO action queue for time-based command execution:

#### Action Queuing

```c
void queue_action(client_t *client, action_t *action)
{
    if (!client || !action)
        return;
    action->next = NULL;
    if (!client->action_queue_tail) {
        client->action_queue_head = action;
        client->action_queue_tail = action;
    } else {
        client->action_queue_tail->next = action;
        client->action_queue_tail = action;
    }
    client->action_queue_count++;
}
```

#### Action Processing

Actions are processed when their execution time is reached:

```c
static int process_ready_action(client_t *client, double now)
{
    action_t *action = client->action_queue_head;
    
    if (!action || action->exec_time > now)
        return 0;
        
    // Dequeue and execute action
    client->action_queue_head = action->next;
    if (!client->action_queue_head)
        client->action_queue_tail = NULL;
    client->action_queue_count--;
    
    execute_action_callback(client, action, now);
    
    // Cleanup
    free(action->command);
    free(action);
    return 1;
}
```

### Time Unit Management

Game time is managed through configurable time units:

```c
double get_time_unit(server_t *server)
{
    if (!server || server->config.freq == 0)
        return 1.0;
    return 1.0 / (double)server->config.freq;
}
```

---

## Error Handling

### Network Error Management

The implementation handles various network error conditions:

#### Connection Errors

- **Accept failures**: Logged and continue operation
- **Socket errors**: Graceful client disconnection
- **Buffer allocation failures**: Connection termination

#### Message Processing Errors

- **Incomplete messages**: Buffered until complete
- **Invalid commands**: Appropriate error responses sent
- **Protocol violations**: Client disconnection

### Resource Management

#### Memory Management

All dynamic allocations are tracked and properly freed:

- Client buffers are freed on disconnection
- Action queues are completely drained during cleanup
- Socket file descriptors are closed systematically

#### File Descriptor Management

File descriptors are managed carefully to prevent leaks:

- Server socket closed during shutdown
- Client sockets closed immediately upon disconnection
- Signal handling file descriptors cleaned up properly

---

## Performance Considerations

### Scalability Features

#### Memory Efficiency

- **Dynamic buffers**: Expand only when necessary, starting at 4KB
- **Client capacity**: Configurable maximum of 100 concurrent clients
- **Array compaction**: Maintains memory locality for client iteration

#### I/O Optimization

- **Non-blocking sockets**: Prevents blocking on slow clients
- **Poll-based multiplexing**: Efficient handling of multiple connections
- **Minimal system calls**: Batch processing where possible

### Bottleneck Prevention

#### Action Queue Limits

- **Maximum queue size**: 50 actions per client prevents memory exhaustion
- **Time-based processing**: Prevents command flooding
- **Queue overflow handling**: Reject excess commands gracefully

#### Message Size Limits

- **Buffer growth strategy**: Doubling strategy balances memory and performance
- **Maximum message size**: Implicitly limited by available memory
- **Partial message handling**: Prevents incomplete command execution

---

## Implementation Guidelines

### Adding New Network Features

#### Protocol Extensions

1. **Command Addition**: Add new entries to command dispatch tables
2. **Handler Implementation**: Implement command-specific handler functions
3. **Response Formatting**: Follow existing message format conventions
4. **Error Handling**: Implement appropriate error responses

#### Client Type Extensions

1. **Type Definition**: Add new client type to `client_type_t` enum
2. **Authentication**: Extend authentication logic for new client types
3. **Protocol Handler**: Implement protocol-specific message handling
4. **State Management**: Add client-specific state management

### Debugging and Monitoring

#### Logging Strategies

- **Connection Events**: Log client connections and disconnections
- **Protocol Errors**: Log invalid commands and protocol violations
- **Performance Metrics**: Track client counts and message rates
- **Resource Usage**: Monitor memory allocation and file descriptor usage

#### Testing Approaches

- **Load Testing**: Test with maximum client connections
- **Protocol Compliance**: Verify command parsing and response formatting
- **Error Injection**: Test error handling paths
- **Memory Testing**: Verify proper resource cleanup

### Maintenance Procedures

#### Regular Maintenance

- **Memory Leak Detection**: Use tools like Valgrind for memory profiling
- **Performance Profiling**: Monitor CPU usage and response times
- **Log Rotation**: Implement log management for production deployments
- **Configuration Tuning**: Adjust buffer sizes and limits based on usage patterns

#### Code Quality Standards

The network implementation follows strict coding standards:

- **Naming Conventions**: Function names use snake_case with module prefixes
- **Error Handling**: All system calls checked for error conditions
- **Memory Management**: Consistent allocation and deallocation patterns
- **Documentation**: Comprehensive function and structure documentation

This network documentation provides the foundation for understanding, modifying, and extending the Zappy server network implementation. The modular design and clear separation of concerns facilitate both maintenance and feature development.