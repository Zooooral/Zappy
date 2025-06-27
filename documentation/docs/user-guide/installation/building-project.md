---
sidebar_position: 2
---

# Building the Project

## Build Process Overview

The Zappy project uses a unified Makefile that handles compilation of all three components with proper dependency management and parallel compilation support.

## Quick Build

```bash
# Clone the repository
git clone <repository-url>
cd zappy

# Build all components
make all
```

This command will:
1. Compile the C server with optimizations
2. Build the C++ GUI with Raylib integration
3. Install Node.js dependencies for the AI
4. Copy required assets
5. Build documentation (if enabled)

## Individual Component Building

### Server Only
```bash
make server
```
Produces: `zappy_server` executable

### GUI Only
```bash
make gui
```
Produces: `zappy_gui` executable

### AI Only
```bash
make ai
```
Produces: `zappy_ai` executable script

## Build Configurations

### Debug Build
```bash
make debug
```
- Enables debug symbols (`-g3`)
- Adds debug preprocessor flag (`-DDEBUG`)
- Disables optimizations for better debugging

### Clean Build
```bash
make clean    # Remove object files
make fclean   # Full clean including executables
make re       # Clean and rebuild
```
