# GUI Architecture

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Design Patterns](#design-patterns)
3. [Core Systems](#core-systems)
4. [State Management](#state-management)
5. [Rendering Pipeline](#rendering-pipeline)
6. [Resource Management](#resource-management)
7. [Event Handling](#event-handling)
8. [Performance Considerations](#performance-considerations)
9. [Extension Points](#extension-points)

---

## Architecture Overview

The Zappy GUI implements a layered architecture with clear separation between presentation, logic, and data layers. Built with Raylib and C++17, it provides real-time 3D visualization of the game world with modern UI components.

### System Components

```
┌─────────────────────────────────────────────────────────┐
│                   Application Layer                      │
├─────────────────────────────────────────────────────────┤
│  Game States  │  UI Components  │  Input Handlers      │
├─────────────────────────────────────────────────────────┤
│     Core Systems (Managers & Controllers)               │
├─────────────────────────────────────────────────────────┤
│  Network Layer  │  Rendering  │  Resource Management   │
├─────────────────────────────────────────────────────────┤
│                 Raylib Graphics Engine                  │
└─────────────────────────────────────────────────────────┘
```

### Architectural Principles

**Single Responsibility**: Each class has a focused purpose with minimal dependencies

**Dependency Inversion**: High-level modules depend on abstractions, not concretions

**Observer Pattern**: Event-driven communication between loosely coupled components

**Singleton Management**: Centralized access to critical resources with controlled lifecycle

---

## Design Patterns

### Singleton Pattern

Critical systems use the Singleton pattern for centralized resource management:

```cpp
class NetworkManager {
public:
    static NetworkManager& getInstance();
private:
    NetworkManager() = default;
    // Deleted copy/move constructors
};
```

**Singleton Components:**
- `GameStateManager`: Application state orchestration
- `NetworkManager`: Server communication
- `ConfigManager`: Settings persistence
- `FontManager`: Typography resources
- `SoundManager`: Audio system
- `CameraController`: View management
- `CharacterManager`: Entity management

### State Machine Pattern

Application flow managed through the State Machine pattern:

```cpp
class GameStateManager {
    enum class Transition { NONE, FADE };
    
    std::unordered_map<std::string, std::unique_ptr<IGameState>> _states;
    void changeState(const std::string& name, Transition transition);
};
```

**State Hierarchy:**
- `IGameState` → `AGameState` → Concrete states
- Automatic lifecycle management (`onEnter`, `onExit`)
- Graceful transition handling with visual effects

### Component Pattern

UI system implements component-based architecture:

```cpp
class IComponent {
public:
    virtual void update(float dt) = 0;
    virtual void draw() const = 0;
    virtual bool isHovered() const = 0;
};
```

**Component Inheritance:**
- `IComponent` → `AComponent` → Concrete UI elements
- Uniform update/render lifecycle
- Event handling abstraction

### Observer Pattern

Network events propagated through callback mechanisms:

```cpp
class NetworkManager {
    std::function<void(const std::string&)> _messageCallback;
    void setMessageCallback(std::function<void(const std::string&)> callback);
};
```

---

## Core Systems

### Game State Management

**GameStateManager Architecture:**
- Centralized state registration and transitions
- Exception-safe state changes with rollback capability
- Visual transition effects (fade, immediate)
- Automatic state lifecycle management

**State Lifecycle:**
1. State registration with unique identifiers
2. Transition initiation with optional effects
3. Current state exit procedures
4. New state entry initialization
5. Transition completion and cleanup

### Configuration System

**ConfigManager Features:**
- INI-style configuration file parsing
- Runtime setting modification
- Automatic defaults and validation
- Key binding management
- Network configuration persistence

**Configuration Categories:**
- **Game Settings**: Volume, graphics preferences
- **Key Bindings**: Customizable input mapping
- **Network Settings**: Host, port, connection parameters

### Resource Management

**Centralized Resource Loading:**
- `FontManager`: Typography with multiple sizes and weights
- `SoundManager`: Audio streaming and effects
- `PropManager`: 3D environmental objects
- `CharacterManager`: Animated character models

**Resource Lifecycle:**
1. Lazy loading on first access
2. Reference counting for shared resources
3. Automatic cleanup on application exit
4. Error handling for missing assets

---

## State Management

### Application States

**State Hierarchy:**
```
IGameState
└── AGameState
    ├── SplashScreen      # Initial loading
    ├── MainMenu          # Primary navigation
    ├── SettingsMenu      # Configuration interface
    ├── GameScreen        # Gameplay visualization
    └── EndScreen         # Game conclusion
```

### State Transitions

**Transition Types:**
- **Immediate**: Instant state change
- **Fade**: Smooth alpha blending between states

**Transition Safety:**
- Exception handling with state rollback
- Resource cleanup on failed transitions
- Graceful degradation on critical errors

### Data Flow

**State Data Management:**
- Isolated state data with controlled sharing
- Network data propagation through observers
- UI state synchronization with game data
- Persistent configuration across sessions

---

## Rendering Pipeline

### Dual Rendering Mode

**3D Mode (Primary):**
- Perspective camera with orbital controls
- Real-time 3D world visualization
- Character animation and particle effects
- Environmental object rendering

**2D Mode (Alternative):**
- Orthographic top-down view
- Tile-based grid visualization
- Simplified UI for performance
- Accessibility-focused rendering

### Rendering Architecture

**Frame Rendering Pipeline:**
```cpp
void draw() {
    ClearBackground(BLACK);
    
    // 3D World Rendering
    BeginMode3D(camera);
    drawWorld3D();
    drawCharacters3D();
    drawEffects3D();
    EndMode3D();
    
    // 2D UI Overlay
    drawUserInterface();
    drawDebugInfo();
}
```

**Performance Optimization:**
- Frustum culling for 3D objects
- Level-of-detail for distant entities
- Batch rendering for similar objects
- Efficient text rendering with cached fonts

### Camera System

**CameraController Features:**
- Smooth camera interpolation
- World boundary constraints
- Mouse and keyboard controls
- View mode switching (3D/2D)

**Camera Modes:**
- **Free Camera**: User-controlled movement
- **Follow Mode**: Track specific entities
- **Fixed Views**: Predefined angles

---

## Resource Management

### Memory Management

**RAII Principles:**
- Smart pointers for automatic cleanup
- Exception-safe resource acquisition
- Deterministic resource release
- Minimal manual memory management

**Resource Categories:**
- **Graphics**: Textures, models, shaders
- **Audio**: Music streams, sound effects
- **Data**: Configuration, game state
- **Network**: Connection resources

### Asset Pipeline

**Loading Strategy:**
- On-demand resource loading
- Background asset streaming
- Error recovery for missing files
- Platform-specific resource paths

**Asset Organization:**
```
assets/
├── fonts/           # Typography resources
├── sounds/          # Audio files
├── environment/     # 3D models and textures
└── shaders/         # Graphics shaders
```

---

## Event Handling

### Input System

**Multi-layered Input Processing:**
1. Raw input capture (Raylib)
2. Input mapping through ConfigManager
3. Context-sensitive command dispatch
4. UI component event handling

**Input Categories:**
- **Camera Controls**: Movement, rotation, zoom
- **UI Interaction**: Button clicks, hover states
- **Debug Commands**: Development toggles
- **System Commands**: Exit, fullscreen

### Network Events

**Event Propagation:**
```cpp
// Network message reception
NetworkManager::update() {
    while (!_receiveQueue.empty()) {
        std::string message = _receiveQueue.front();
        _receiveQueue.pop();
        
        if (_protocolHandler) {
            _protocolHandler->handleCommand(message);
        }
        
        if (_messageCallback) {
            _messageCallback(message);
        }
    }
}
```

**Event Types:**
- **Game State Updates**: Player positions, resource changes
- **Connection Events**: Connect, disconnect, authentication
- **Protocol Messages**: Server commands and responses

---

## Performance Considerations

### Optimization Strategies

**Rendering Performance:**
- Object pooling for frequently created entities
- Spatial partitioning for visibility culling
- Texture atlasing for reduced draw calls
- Efficient particle system management

**Memory Optimization:**
- Resource sharing between similar objects
- Lazy loading with reference counting
- Periodic garbage collection of unused assets
- Memory pool allocation for temporary objects

**Network Performance:**
- Message batching for reduced overhead
- Efficient protocol message parsing
- Background thread for network I/O
- Connection pooling and reuse

### Scalability Features

**Dynamic Quality Adjustment:**
- Automatic LOD based on performance
- Quality scaling for different hardware
- Optional visual effects for performance
- Adaptive frame rate management

**Concurrent Processing:**
- Network I/O on dedicated thread
- Asset loading in background
- UI updates independent of game logic
- Thread-safe resource access

---

## Extension Points

### Adding New Game States

**Implementation Steps:**
1. Inherit from `AGameState`
2. Implement required virtual methods
3. Register with `GameStateManager`
4. Handle state transitions

```cpp
class CustomGameState : public AGameState {
public:
    void update(float dt) override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
};
```

### Custom UI Components

**Component Development:**
1. Inherit from `IComponent` or `AComponent`
2. Implement update and draw methods
3. Handle input events appropriately
4. Integrate with existing UI systems

### Protocol Extensions

**Network Protocol Expansion:**
1. Extend `ProtocolHandler` with new command handlers
2. Maintain thread safety in message processing
3. Update network message callbacks
4. Ensure backward compatibility

### Graphics Enhancements

**Rendering Extensions:**
- Custom shader implementation
- New particle effect systems
- Advanced lighting models
- Post-processing effects

**Integration Points:**
- Raylib shader system
- Custom rendering callbacks
- Resource management integration
- Performance monitoring hooks

This architecture provides a solid foundation for the Zappy GUI while maintaining flexibility for future enhancements and academic evaluation requirements.