# GUI Network Architecture

## Table of Contents
1. [Overview](#overview)
2. [Architecture Components](#architecture-components)
3. [Connection Management](#connection-management)
4. [Protocol Handling](#protocol-handling)
5. [Cross-Platform Network Support](#cross-platform-network-support)
6. [Threading Model](#threading-model)
7. [Error Handling and Resilience](#error-handling-and-resilience)
8. [Configuration and Constants](#configuration-and-constants)
9. [Integration Guidelines](#integration-guidelines)
10. [Implementation Guidelines](#implementation-guidelines)

---

## Overview

The Zappy GUI networking subsystem provides real-time communication between the graphical client and the game server. This subsystem is designed with modern C++ practices, emphasizing thread safety, cross-platform compatibility, and maintainable architecture.

### Network Protocol Characteristics
- **Text-based protocol**: Commands and responses in human-readable format
- **Stateful connection**: Persistent TCP connection with authentication sequence
- **Real-time updates**: Continuous stream of game state changes
- **Bidirectional communication**: GUI sends commands and receives server updates

### Design Principles
- **Separation of concerns**: Clear separation between networking, protocol handling, and application logic
- **Thread safety**: All network operations protected with appropriate synchronization
- **Platform abstraction**: Unified interface across Windows and UNIX-like systems
- **Resource management**: Automatic cleanup and proper resource disposal

---

## Architecture Components

### Core Classes

#### NetworkManager
**Purpose**: Singleton class managing the primary network connection to the server

**Key Responsibilities:**
- TCP socket management and lifecycle
- Connection state tracking
- Message queuing and buffering
- Thread coordination
- Authentication sequence handling

**Interface Compliance**: Implements `INetworkClient` interface

#### ProtocolHandler
**Purpose**: Processes incoming server messages and translates them into application events

**Key Responsibilities:**
- Command parsing and validation
- Message routing to appropriate handlers
- Protocol compliance verification
- Callback management for different message types

#### NetworkPlatform
**Purpose**: Cross-platform networking abstraction layer

**Key Responsibilities:**
- Platform-specific socket operations
- Error code translation
- Network initialization and cleanup
- Socket configuration utilities

### Interface Definitions

#### INetworkClient
**Purpose**: Abstract interface defining network client contract

```cpp
class INetworkClient {
public:
    virtual bool connectToServer(const std::string& host, int port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual void sendCommand(const std::string& command) = 0;
    virtual void setMessageCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void update() = 0;
};
```

---

## Connection Management

### Connection States

The network system maintains precise connection state tracking through the `ConnectionState` enumeration:

| State | Description | Transitions |
|-------|-------------|-------------|
| `DISCONNECTED` | No active connection | → `CONNECTING` |
| `CONNECTING` | Connection attempt in progress | → `CONNECTED`, `ERROR` |
| `CONNECTED` | TCP connection established | → `AUTHENTICATED`, `ERROR` |
| `AUTHENTICATED` | Server handshake completed | → `DISCONNECTED`, `ERROR` |
| `ERROR` | Connection failure occurred | → `DISCONNECTED` |

### Authentication Sequence

The GUI client follows the standard Zappy authentication protocol:

```
1. Client establishes TCP connection
2. Server sends: "WELCOME"
3. Client responds: "GRAPHIC"
4. Server sends map size: "msz X Y"
5. Connection transitions to AUTHENTICATED state
```

### Connection Lifecycle

**Initialization:**
1. Socket creation and configuration
2. Hostname resolution
3. TCP connection establishment
4. Background thread startup
5. Authentication sequence execution

**Operation:**
1. Continuous message processing
2. Queue management
3. State synchronization
4. Error detection and recovery

**Termination:**
1. Graceful thread shutdown
2. Socket closure
3. Resource cleanup
4. Queue purging

---

## Protocol Handling

### Message Processing Pipeline

**Incoming Messages:**
1. Raw data reception in network thread
2. Message boundary detection (newline-based)
3. Queue insertion with thread synchronization
4. Protocol handler processing
5. Application callback invocation

**Outgoing Messages:**
1. Command formatting and validation
2. Queue insertion with newline termination
3. Asynchronous transmission
4. Delivery confirmation

### Protocol Command Support

The GUI client supports the complete Zappy graphic protocol:

#### Server Information Commands
- **msz**: Map size specification
- **bct**: Individual tile content
- **mct**: Complete map content
- **tna**: Team name listing

#### Player State Commands
- **pnw**: New player notification
- **ppo**: Player position updates
- **plv**: Player level changes
- **pin**: Player inventory status

#### Game Event Commands
- **pic**: Incantation start notification
- **pie**: Incantation completion
- **pex**: Player expulsion
- **pbc**: Player broadcast message
- **pfk**: Egg laying event
- **pdr**: Resource dropping
- **pgt**: Resource collection
- **pdi**: Player death
- **enw**: Egg placement
- **ebo**: Egg connection
- **edi**: Egg destruction
- **seg**: Game termination

#### Administrative Commands
- **sgt**: Time unit query
- **sst**: Time unit modification

### Message Format Validation

Each protocol handler implements strict message format validation:

```cpp
void handleMapSize(const std::string& args) {
    std::istringstream iss(args);
    int width, height;
    if (iss >> width >> height && _mapSizeCallback) {
        _mapSizeCallback(width, height);
    }
}
```

---

## Cross-Platform Network Support

### Platform Abstraction

The `NetworkPlatform` class provides unified networking interfaces across operating systems:

#### Windows Implementation
- **Winsock2 API**: Native Windows socket support
- **WSAStartup/WSACleanup**: Network subsystem initialization
- **WSAGetLastError**: Windows-specific error reporting
- **SOCKET type**: Windows socket handle type

#### UNIX Implementation
- **BSD Sockets**: Standard POSIX socket interface
- **errno**: UNIX error reporting mechanism
- **int type**: UNIX file descriptor socket type
- **fcntl**: Socket configuration operations

### Error Handling Translation

Platform-specific error codes are translated to readable strings:

```cpp
std::string NetworkPlatform::getErrorString(int error) {
#ifdef _WIN32
    // Windows: FormatMessageA for detailed error descriptions
#else
    // UNIX: strerror for system error messages
#endif
}
```

### Socket Configuration

Common socket operations abstracted across platforms:

- **Non-blocking mode**: Platform-specific socket flags
- **Socket closure**: Unified close operation
- **Address resolution**: Cross-platform hostname lookup

---

## Threading Model

### Background Network Thread

The network subsystem operates a dedicated background thread for I/O operations:

**Thread Responsibilities:**
- Socket monitoring with select()
- Data transmission and reception
- Connection state management
- Queue processing

**Synchronization Mechanisms:**
- **Atomic operations**: Connection state updates
- **Mutex protection**: Queue access and modification
- **Condition variables**: Graceful shutdown coordination

### Thread Safety Design

**Queue Protection:**
```cpp
std::queue<std::string> _sendQueue;
std::queue<std::string> _receiveQueue;
mutable std::mutex _sendQueueMutex;
mutable std::mutex _receiveQueueMutex;
```

**Callback Synchronization:**
```cpp
mutable std::mutex _callbackMutex;
std::function<void(const std::string&)> _messageCallback;
```

**State Management:**
```cpp
std::atomic<ConnectionState> _connectionState;
std::atomic<bool> _running;
```

### Main Thread Integration

The main application thread interacts with the network subsystem through:

**Non-blocking Operations:**
- Command queuing via `sendCommand()`
- Message retrieval via `update()`
- State queries via `getConnectionState()`

**Callback Registration:**
- Message callbacks for protocol events
- Protocol handler registration for specialized processing

---

## Error Handling and Resilience

### Connection Resilience

**Automatic Reconnection**: Not implemented in current version - requires manual reconnection

**Error Detection:**
- Socket error monitoring
- Connection timeout detection
- Protocol violation identification

**Recovery Strategies:**
- Graceful degradation on connection loss
- Resource cleanup on error conditions
- State reset for reconnection attempts

### Error Categories

#### Network Errors
- **Connection refused**: Server unavailable
- **Timeout**: Server unresponsive
- **Socket errors**: Low-level network issues

#### Protocol Errors
- **Malformed messages**: Invalid command format
- **Authentication failure**: Server rejection
- **Unexpected responses**: Protocol sequence violations

#### Resource Errors
- **Memory allocation**: Queue overflow conditions
- **Thread creation**: System resource exhaustion
- **File descriptor**: Socket creation failures

### Error Reporting

**Logging Integration:**
```cpp
std::cerr << "Protocol handler error: " << e.what() << std::endl;
```

**Exception Handling:**
```cpp
try {
    _protocolHandler->handleCommand(message);
} catch (const std::exception& e) {
    // Error logging and recovery
}
```

---

## Configuration and Constants

### Network Configuration

Configuration values defined in `Constants.hpp`:

```cpp
namespace zappy {
namespace constants {
    constexpr int DEFAULT_PORT = 4242;
    constexpr const char* DEFAULT_HOST = "localhost";
    constexpr int NETWORK_BUFFER_SIZE = 1024;
    constexpr int NETWORK_TIMEOUT_US = 100000;
}
}
```

### Configurable Parameters

#### Connection Settings
- **Default port**: 4242 (Zappy standard)
- **Default host**: localhost
- **Connection timeout**: 10 seconds
- **Buffer size**: 1024 bytes

#### Performance Tuning
- **Select timeout**: 100ms
- **Queue capacity**: Unlimited (memory-constrained)
- **Thread priority**: Standard priority

### Runtime Configuration

The system supports runtime configuration through:

**Environment variables**: Via development tooling
**Command-line arguments**: Host and port specification
**Configuration files**: Not currently implemented

---

## Integration Guidelines

### Basic Integration

**Singleton Access:**
```cpp
NetworkManager& network = NetworkManager::getInstance();
```

**Connection Establishment:**
```cpp
if (!network.connectToServer("localhost", 4242)) {
    // Handle connection failure
}
```

**Message Callback Registration:**
```cpp
network.setMessageCallback([](const std::string& message) {
    // Process received message
});
```

**Command Transmission:**
```cpp
network.sendCommand("msz");  // Request map size
```

**Update Processing:**
```cpp
network.update();  // Process queued messages
```

### Advanced Integration

**Protocol Handler Registration:**
```cpp
auto protocolHandler = std::make_shared<ProtocolHandler>();
protocolHandler->setMapSizeCallback([](int width, int height) {
    // Handle map size update
});
network.setProtocolHandler(protocolHandler);
```

**Connection State Monitoring:**
```cpp
switch (network.getConnectionState()) {
    case ConnectionState::AUTHENTICATED:
        // Ready for game communication
        break;
    case ConnectionState::ERROR:
        // Handle connection error
        break;
}
```

### Application Lifecycle Integration

**Startup Sequence:**
1. Network platform initialization
2. NetworkManager connection
3. Authentication waiting
4. Protocol handler setup
5. Application state initialization

**Main Loop Integration:**
```cpp
while (applicationRunning) {
    network.update();           // Process network messages
    updateGameState();          // Update application state
    renderFrame();              // Render current frame
}
```

**Shutdown Sequence:**
1. Network disconnection
2. Thread synchronization
3. Resource cleanup
4. Platform cleanup

---

## Implementation Guidelines

### Best Practices

#### Thread Safety
- Always use mutex protection for shared data structures
- Prefer atomic operations for simple state variables
- Avoid blocking operations in main thread

#### Resource Management
- Implement RAII principles for network resources
- Use smart pointers for protocol handlers
- Ensure proper cleanup in destructors

#### Error Handling
- Implement comprehensive error checking
- Provide meaningful error messages
- Design for graceful degradation

#### Performance Optimization
- Minimize memory allocations in hot paths
- Use efficient data structures for queues
- Implement appropriate buffer sizes

### Extension Points

#### Custom Protocol Handlers
Extend `ProtocolHandler` for specialized message processing:

```cpp
class CustomProtocolHandler : public ProtocolHandler {
public:
    void handleCustomCommand(const std::string& command) override {
        // Custom command processing
    }
};
```

#### Network Monitoring
Implement network statistics and monitoring:

```cpp
class NetworkMonitor {
public:
    void recordMessageReceived(const std::string& message);
    void recordMessageSent(const std::string& message);
    NetworkStatistics getStatistics() const;
};
```

#### Alternative Transport Layers
The interface design supports alternative network implementations:

```cpp
class UDPNetworkClient : public INetworkClient {
    // UDP-based implementation
};
```

### Testing Considerations

#### Unit Testing
- Mock `INetworkClient` interface for testing
- Test protocol handlers with known message formats
- Verify error handling with simulated failures

#### Integration Testing
- Test with actual Zappy server instances
- Verify protocol compliance
- Test connection recovery scenarios

#### Performance Testing
- Measure throughput under load
- Test with large message volumes
- Verify memory usage patterns

### Development Tools

#### Network Debugging
- Enable debug logging for protocol messages
- Use network sniffing tools for protocol analysis
- Implement network simulation for testing

#### Configuration Management
- Support multiple server configurations
- Enable runtime parameter modification
- Provide configuration validation

This documentation provides comprehensive coverage of the Zappy GUI networking architecture, enabling developers to understand, maintain, and extend the networking subsystem effectively.
