---
sidebar_position: 3
---

# Quick Start Guide

## Installation Overview

The Zappy project consists of three main components that work together to create a complete game experience. This guide will get you up and running quickly.

## Step 1: Clone and Build

```bash
# Clone the repository
git clone git@github.com:EpitechPromo2028/B-YEP-400-PAR-4-1-zappy-maxence.bunel.git
cd zappy

# Build all components
make all
```

This single command builds:
- `zappy_server` - The game server (C)
- `zappy_gui` - The visualization client (C++)
- `zappy_ai` - The AI client (Node.js)

## Step 2: Start the Server

```bash
# Basic server startup
./zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 5 -f 100
```

Parameters explained:
- `-p 4242`: Server port
- `-x 10 -y 10`: World dimensions (10x10)
- `-n team1 team2`: Team names
- `-c 5`: Max clients per team
- `-f 100`: Time frequency (ticks per second)

## Step 3: Launch the GUI

```bash
# Connect to local server
./zappy_gui -p 4242 -h localhost
```

The GUI will connect and display the game world in real-time.

## Step 4: Start AI Clients

```bash
# Start first AI client
./zappy_ai -p 4242 -n team1 -h localhost

# Start second AI client (in another terminal)
./zappy_ai -p 4242 -n team2 -h localhost
```

## Verification

You should now see:
1. Server console showing client connections
2. GUI displaying the game world with players
3. AI clients moving and interacting autonomously

## Next Steps

- Read the [Game Rules](../gameplay/rules) to understand the objective
- Explore [Advanced Configuration](advanced-setup) for custom setups

## Common Commands

```bash
# Clean build
make fclean && make all

# Debug build
make debug

# Server with verbose output
./zappy_server -p 4242 -x 20 -y 20 -n red blue green -c 10 -f 50 -v

# GUI with specific resolution
./zappy_gui -p 4242 -h localhost --width 1920 --height 1080
```
