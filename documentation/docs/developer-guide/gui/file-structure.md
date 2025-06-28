# GUI File Structure

## Overview

The Zappy GUI follows a modular C++ architecture with clear separation of concerns. The codebase is organized into logical modules that promote maintainability, extensibility, and code reuse.

---

## Directory Structure

```
src/gui/
├── main.cpp                    # Application entry point
├── core/                       # Core systems and managers
├── network/                    # Network communication layer
├── ui/                        # User interface components
├── screens/                   # Game state screens
├── entities/                  # Game world entities
└── interfaces/                # Abstract interfaces

include/                       # Header files (if separate)
assets/                       # Resources and assets
build/                        # Build artifacts
```

---

## Module Breakdown

### Core Module (`src/gui/core/`)

**Purpose**: Fundamental systems and application management

**Key Components**:
- **AGameState.hpp**: Abstract base class for all game states
- **GameStateManager.hpp**: Singleton managing state transitions
- **ConfigManager.hpp**: Configuration loading and persistence
- **Constants.hpp**: Application-wide constants and settings
- **FontManager.hpp**: Font loading and management
- **SoundManager.hpp**: Audio system management
- **GameWorld.hpp**: World state representation
- **Environment.hpp**: Environmental systems
- **CameraController.hpp**: 3D camera management
- **DebugSystem.cpp**: Development debugging utilities
- **ArgumentValidator.hpp**: Command-line argument processing

### Network Module (`src/gui/network/`)

**Purpose**: Server communication and protocol handling

**Key Components**:
- **NetworkManager.hpp/.cpp**: Main network connection manager
- **ProtocolHandler.hpp/.cpp**: Zappy protocol message processing
- **NetworkPlatform.hpp/.cpp**: Cross-platform socket abstraction

**Architecture Pattern**: Singleton + Observer pattern for network events

### UI Module (`src/gui/ui/`)

**Purpose**: User interface components and widgets

**Key Components**:
- **IComponent.hpp**: Base interface for UI elements
- **Button.hpp**: Interactive button component
- **Dashboard.hpp**: Game information display
- **InventoryUI.hpp**: Player inventory interface
- **KeyBindButton.hpp**: Key binding configuration

**Design Pattern**: Component-based UI with interface inheritance

### Screens Module (`src/gui/screens/`)

**Purpose**: Complete application screens and states

**Key Components**:
- **SplashScreen.hpp**: Initial loading screen
- **MainMenu.hpp**: Main menu interface
- **GameScreen.hpp**: Primary gameplay state
- **SettingsMenu.hpp**: Configuration interface
- **EndScreen.hpp**: Game conclusion display

**Pattern**: State machine implementation extending AGameState

### Entities Module (`src/gui/entities/`)

**Purpose**: Game world objects and their management

**Key Components**:
- **CharacterManager.cpp**: Player character rendering
- **EggManager.hpp**: Egg object lifecycle
- **PropManager.cpp**: Environmental object management

**Pattern**: Manager classes for entity lifecycle and rendering

### Interfaces Module (`src/gui/interfaces/`)

**Purpose**: Abstract contracts and interface definitions

**Key Components**:
- **IGameState.hpp**: Game state contract
- **INetworkClient.hpp**: Network client interface
- **IProtocolHandler.hpp**: Protocol processing interface
- **IComponent.hpp**: UI component interface

**Pattern**: Interface segregation principle implementation

---

## Naming Conventions

### Classes and Files
- **Abstract Classes**: Prefix with `A` (e.g., `AGameState`)
- **Interfaces**: Prefix with `I` (e.g., `INetworkClient`)
- **Managers**: Suffix with `Manager` (e.g., `NetworkManager`)
- **Header Files**: `.hpp` extension for C++ headers
- **Source Files**: `.cpp` extension for implementations

### Code Organization
- **One class per file**: Each class has dedicated header and source files
- **Header guards**: All headers use `#ifndef` guards
- **Namespace usage**: `zappy` namespace for project-specific code

---

## Build Organization

### Build Directories
```
build/gui/
├── core/           # Core module objects
├── network/        # Network module objects
├── ui/             # UI module objects
├── entities/       # Entities module objects
└── screens/        # Screens module objects
```

### Dependencies
- **External**: Raylib (graphics), system networking libraries
- **Internal**: Clear module dependencies with minimal coupling
- **Platform**: Cross-platform support (Windows/Linux)

---

## Development Guidelines

### Adding New Components

**UI Components**:
1. Inherit from `IComponent`
2. Place in `src/gui/ui/`
3. Update build configuration

**Game States**:
1. Inherit from `AGameState`
2. Place in `src/gui/screens/`
3. Register with `GameStateManager`

**Network Features**:
1. Extend `ProtocolHandler` for new commands
2. Maintain thread safety
3. Follow existing patterns

### Module Dependencies

**Dependency Flow**:
```
main.cpp
├── core/ (foundational)
├── network/ (depends on core)
├── ui/ (depends on core)
├── screens/ (depends on core, ui, network)
├── entities/ (depends on core)
└── interfaces/ (independent)
```

### File Inclusion Strategy
- **Prefer forward declarations** in headers when possible
- **Include minimal dependencies** to reduce compilation time
- **Use interface headers** to decouple implementation details

---

## Configuration Files

### Project Configuration
- **Makefile**: Main build configuration
- **build_gui.bat**: Windows build script
- **.gitignore**: Version control exclusions

### Runtime Configuration
- **Constants.hpp**: Compile-time configuration
- **ConfigManager**: Runtime settings persistence
- **ArgumentValidator**: Command-line configuration

This file structure promotes clean architecture principles with clear separation of concerns, making the codebase maintainable and extensible for academic evaluation and future development.
