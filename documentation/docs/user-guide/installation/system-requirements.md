---
sidebar_position: 1
---

# System Requirements

## Hardware Requirements

### Minimum Requirements
- **CPU**: Dual-core processor 2.0 GHz or equivalent
- **Memory**: 2 GB RAM
- **Graphics**: OpenGL 3.3 compatible graphics card
- **Network**: Stable internet connection for multiplayer functionality
- **Storage**: 100 MB available space

### Recommended Requirements
- **CPU**: Quad-core processor 3.0 GHz or equivalent
- **Memory**: 4 GB RAM
- **Graphics**: Dedicated graphics card with OpenGL 4.0 support
- **Network**: Broadband internet connection
- **Storage**: 200 MB available space

## Software Dependencies

### All Platforms
- **Build System**: CMake 3.10+ or Make
- **Network**: TCP/IP stack support

### Server Component (C)
- **Compiler**: GCC 9.0+ or Clang 10.0+
- **Standard**: C11 compliance required
- **Libraries**: POSIX.1-2017 compatible system
- **Threading**: pthread support

### GUI Component (C++)
- **Compiler**: GCC 9.0+ or Clang 10.0+ with C++17 support
- **Graphics Library**: Raylib 4.0+
- **Dependencies**: OpenGL 3.3+ drivers

### AI Component (Node.js)
- **Runtime**: Node.js 20.0+ or 22.0+ (LTS recommended)
- **Package Manager**: npm 10.0+ or yarn 1.22+

## Platform Support

### Linux
- **Distributions**: Ubuntu 20.04+, Debian 11+, CentOS 8+, Arch Linux
- **Package Managers**: apt, yum, pacman support available
- **Development Tools**: build-essential package recommended

### Windows
- **Versions**: Windows 10 1909+ or Windows 11
- **Development Environment**: MinGW-w64 or Visual Studio 2019+
- **Build Tools**: Windows SDK for networking components

### macOS
- **Versions**: macOS 10.15+ (Catalina and later)
- **Development Tools**: Xcode Command Line Tools
- **Package Manager**: Homebrew recommended for dependencies

## Network Configuration

### Firewall Requirements
The following ports must be accessible:
- **Default Server Port**: 4242 (configurable)
- **Protocol**: TCP for all communications
- **Direction**: Inbound for server, outbound for clients

### Network Performance
- **Latency**: Less than 100ms recommended for optimal gameplay
- **Bandwidth**: Minimal requirements (approximately 1 KB/s per client)
- **Concurrent Connections**: Server supports up to 100 simultaneous clients

## Verification

After installation, verify your system meets requirements:

```bash
# Check compiler versions
gcc --version
g++ --version
node --version

# Verify OpenGL support (Linux)
glxinfo | grep "OpenGL version"

# Test network connectivity
telnet localhost 4242
```
