---
sidebar_position: 1
---

# Zappy Documentation

Zappy is a multi-language network game implementation featuring a distributed architecture with three core components: a C server managing game logic and client connections, a C++ GUI providing real-time visualization, and Node.js AI clients implementing autonomous gameplay strategies.

## Architecture Overview

The system follows a client-server architecture where multiple AI clients connect to a central server. The GUI acts as an observer, receiving game state updates for visualization purposes.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  AI Client  │────▶│   Server    │◀────│    GUI      │
│  (Node.js)  │     │     (C)     │     │   (C++)     │
└─────────────┘     └─────────────┘     └─────────────┘
      │                     │                     │
   Commands              Game Logic          Visualization
   Responses             State Mgmt          Real-time Updates
```

## Technical Stack

| Component | Technology | Purpose |
|-----------|------------|---------|
| Server | C (C11 standard) | Game logic, network handling, performance |
| GUI | C++17 with Raylib | 3D visualization, user interface |
| AI | Node.js | Strategy implementation, rapid development |

## Quick Start

```bash
# Build all components
make all

# Start server (example parameters)
./zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 2 -f 100

# Start GUI
./zappy_gui -p 4242 -h localhost

# Start AI client
./zappy_ai -p 4242 -n team1 -h localhost
```

## Documentation Structure

This documentation is organized into two main sections:

**User Guide**: Installation procedures, game rules, and basic usage instructions for end users.

**Developer Guide**: Comprehensive technical documentation for developers working on or extending the codebase.
