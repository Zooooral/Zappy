---
sidebar_position: 1
---

# System Requirements

## Prerequisites

### Core Dependencies
- **GCC**: Version 9.0 or higher (C11 support required)
- **G++**: Version 9.0 or higher (C++17 support required)
- **Node.js**: Version 16.0 or higher
- **NPM**: Version 8.0 or higher
- **Make**: GNU Make 4.0 or higher

### GUI Dependencies
- **Raylib**: Graphics library for 3D rendering
- **OpenGL**: Version 3.3 or higher
- **X11 development libraries** (Linux only)

### Development Tools (Optional)
- **GDB**: For debugging C/C++ components
- **Valgrind**: Memory leak detection
- **Node.js debugging tools**: For AI development

## Platform Support

| Platform | Support Level | Notes |
|----------|---------------|-------|
| Linux (Ubuntu 20.04+) | Full | Primary development platform |
| Linux (Other distros) | Partial | Manual dependency installation required |
| macOS | Partial | Requires Homebrew for dependencies |
| Windows | Unsupported | Use WSL2 or Docker |

## Hardware Requirements

### Minimum
- **CPU**: Dual-core 2.0 GHz
- **RAM**: 4 GB
- **Storage**: 1 GB free space
- **Graphics**: OpenGL 3.3 compatible

### Recommended
- **CPU**: Quad-core 3.0 GHz
- **RAM**: 8 GB
- **Storage**: 2 GB free space
- **Graphics**: Dedicated GPU with OpenGL 4.0+

## Installation Commands

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libraylib-dev nodejs npm
```

### Arch Linux
```bash
sudo pacman -S base-devel raylib nodejs npm
```

### macOS
```bash
brew install raylib node npm
```
