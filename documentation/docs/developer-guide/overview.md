---
sidebar_position: 1
---

# Developer Guide Overview

## Architecture Philosophy

Zappy implements a distributed system architecture with clear separation of concerns:

- **Server (C)**: Authoritative game state and high-performance networking
- **GUI (C++)**: Real-time visualization with modern graphics
- **AI (Node.js)**: Rapid strategy development and testing

## Design Principles

### Performance First
- C server handles concurrent connections efficiently
- Minimal memory allocation in critical paths
- Optimized data structures for game operations

### Maintainable Code
- Clear module separation and interfaces
- Consistent coding standards across languages
- Comprehensive error handling and logging

### Extensible Architecture
- Plugin-based AI strategy system
- Modular GUI components
- Configurable server parameters

## Development Workflow

1. **Setup**: Follow installation and build instructions
2. **Component Development**: Work on individual modules
3. **Integration Testing**: Test component interactions
4. **Performance Profiling**: Optimize critical paths
5. **Documentation**: Update relevant documentation

## Getting Started

New developers should:

1. Read the architecture documentation for their target component
2. Set up the development environment following build instructions
3. Review the coding standards and contribution guidelines
4. Start with small modifications to understand the codebase
5. Use the testing framework to validate changes
