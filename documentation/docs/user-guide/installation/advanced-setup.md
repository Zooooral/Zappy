---
sidebar_position: 4
---

# Advanced Setup

## Custom Build Configuration

### Environment Variables

You can customize the build process using environment variables:

```bash
# Custom compiler
export CC=clang
export CXX=clang++

# Custom build flags
export CFLAGS="-O3 -march=native"
export CXXFLAGS="-O3 -march=native"

# Custom paths
export RAYLIB_PATH=/usr/local/lib/raylib
export NODE_PATH=/usr/local/bin/node

# Build with custom settings
make all
```

### Build Variants

#### Debug Build
```bash
make debug
```
- Enables debug symbols (-g3)
- Adds debug preprocessor definitions
- Disables optimizations for easier debugging
- Includes additional runtime checks

#### Release Build
```bash
make RELEASE=1 all
```
- Maximum optimization (-O3)
- Stripped symbols for smaller binaries
- Disabled assertions
- Production-ready configuration

#### Profile Build
```bash
make PROFILE=1 all
```
- Optimization with debug info
- Profiling instrumentation enabled
- Performance analysis support

## Network Configuration

### Firewall Setup

#### Linux (iptables)
```bash
# Allow server port
sudo iptables -A INPUT -p tcp --dport 4242 -j ACCEPT

# Allow outbound connections
sudo iptables -A OUTPUT -p tcp --dport 4242 -j ACCEPT
```

#### Linux (ufw)
```bash
sudo ufw allow 4242/tcp
```

#### Windows Firewall
```powershell
# PowerShell as Administrator
New-NetFirewallRule -DisplayName "Zappy Server" -Direction Inbound -Protocol TCP -LocalPort 4242 -Action Allow
```

### Load Balancing

For multiple server instances:

```bash
# Server cluster configuration
./zappy_server --port 4242 --cluster-id 1 --cluster-size 3
./zappy_server --port 4243 --cluster-id 2 --cluster-size 3
./zappy_server --port 4244 --cluster-id 3 --cluster-size 3
```

## Development Setup

### IDE Configuration

#### Visual Studio Code
Required extensions:
- C/C++ Extension Pack
- Node.js Extension Pack
- GitLens
- Better C++ Syntax

#### CLion
CMake configuration for better IDE support:
```cmake
cmake_minimum_required(VERSION 3.10)
project(zappy)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add source directories
add_subdirectory(src/server)
add_subdirectory(src/gui)
add_subdirectory(src/ai)
```

This advanced setup guide covers enterprise-level deployment scenarios and development workflows for the Zappy project.
