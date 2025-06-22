#!/bin/bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

NODE_VERSION="20"
RAYLIB_DIR="$HOME/raylib"
PACKAGE_MANAGER=""

print_header() {
    echo -e "${BLUE}"
    echo "██████╗  █████╗ ██████╗ ██████╗ ██╗   ██╗"
    echo "╚════██╗██╔══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝"
    echo " █████╔╝███████║██████╔╝██████╔╝ ╚████╔╝ "
    echo "██╔═══╝ ██╔══██║██╔═══╝ ██╔═══╝   ╚██╔╝  "
    echo "███████╗██║  ██║██║     ██║        ██║   "
    echo "╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝        ╚═╝   "
    echo -e "${NC}"
    echo -e "${GREEN}🚀 Zappy Development Environment Setup${NC}"
    echo ""
}

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[⚠]${NC} $1"
}

log_error() {
    echo -e "${RED}[✗]${NC} $1"
}

check_command() {
    if command -v "$1" >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

detect_package_manager() {
    if check_command "apt"; then
        PACKAGE_MANAGER="apt"
    elif check_command "dnf"; then
        PACKAGE_MANAGER="dnf"
    elif check_command "pacman"; then
        PACKAGE_MANAGER="pacman"
    else
        log_error "Unsupported package manager. Please install dependencies manually."
        exit 1
    fi
}

install_system_deps() {
    log_info "Installing system dependencies..."
    
    case "$PACKAGE_MANAGER" in
        "apt")
            sudo apt update
            sudo apt install -y \
                build-essential \
                cmake \
                git \
                curl \
                libasound2-dev \
                mesa-common-dev \
                libx11-dev \
                libxrandr-dev \
                libxi-dev \
                xorg-dev \
                libgl1-mesa-dev \
                libglu1-mesa-dev \
                tmux \
                inotify-tools
            ;;
        "dnf")
            sudo dnf install -y \
                gcc \
                gcc-c++ \
                cmake \
                git \
                curl \
                alsa-lib-devel \
                mesa-libGL-devel \
                libX11-devel \
                libXrandr-devel \
                libXi-devel \
                libXcursor-devel \
                libXinerama-devel \
                tmux \
                inotify-tools
            ;;
        "pacman")
            sudo pacman -S --noconfirm \
                base-devel \
                cmake \
                git \
                curl \
                alsa-lib \
                mesa \
                libx11 \
                libxrandr \
                libxi \
                libxcursor \
                libxinerama \
                tmux \
                inotify-tools
            ;;
    esac
    
    log_success "System dependencies installed"
}

install_nodejs() {
    log_info "Installing Node.js LTS..."
    
    if check_command "node"; then
        NODE_CURRENT=$(node --version | cut -d'v' -f2 | cut -d'.' -f1)
        if [ "$NODE_CURRENT" -ge "$NODE_VERSION" ]; then
            log_success "Node.js $NODE_CURRENT already installed"
            return 0
        fi
    fi
    
    case "$PACKAGE_MANAGER" in
        "apt")
            curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash -
            sudo apt-get install -y nodejs
            ;;
        "dnf")
            curl -fsSL https://rpm.nodesource.com/setup_lts.x | sudo bash -
            sudo dnf install -y nodejs
            ;;
        "pacman")
            sudo pacman -S --noconfirm nodejs npm
            ;;
    esac
    
    if check_command "node"; then
        log_success "Node.js $(node --version) installed"
    else
        log_error "Failed to install Node.js"
        exit 1
    fi
}

install_raylib() {
    log_info "Installing Raylib..."
    
    if pkg-config --exists raylib; then
        log_success "Raylib is already installed (found via pkg-config)"
        return 0
    fi
    
    mkdir -p "$RAYLIB_DIR"
    cd "$RAYLIB_DIR"
    
    if [ ! -d "raylib" ]; then
        git clone https://github.com/raysan5/raylib.git
    fi
    
    cd raylib
    mkdir -p build
    cd build
    
    cmake .. -DCMAKE_INSTALL_PREFIX="$RAYLIB_DIR" -DBUILD_EXAMPLES=OFF
    make -j$(nproc)
    sudo make install
    
    echo "export PKG_CONFIG_PATH=\"$RAYLIB_DIR/lib/pkgconfig:\$PKG_CONFIG_PATH\"" >> ~/.bashrc
    echo "export LD_LIBRARY_PATH=\"$RAYLIB_DIR/lib:\$LD_LIBRARY_PATH\"" >> ~/.bashrc
    
    log_success "Raylib installed at $RAYLIB_DIR"
}

setup_development_scripts() {
    log_info "Setting up development scripts..."
    
    chmod +x ./*.sh 2>/dev/null || true
    
    log_success "Development scripts configured"
}

verify_installation() {
    log_info "Verifying installation..."
    
    if check_command "node"; then
        log_success "Node.js: $(node --version)"
    else
        log_error "Node.js not found"
        return 1
    fi
    
    if check_command "npm"; then
        log_success "npm: $(npm --version)"
    else
        log_error "npm not found"
        return 1
    fi
    
    if pkg-config --exists raylib; then
        log_success "Raylib: Installed at $RAYLIB_DIR"
    else
        log_error "Raylib not found"
        return 1
    fi
    
    export PKG_CONFIG_PATH="$RAYLIB_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"
    if pkg-config --exists raylib; then
        log_success "Raylib pkg-config: OK"
    else
        log_warning "Raylib pkg-config not found, you may need to restart your shell"
    fi
}

main() {
    print_header
    
    log_info "Starting Zappy development environment setup..."
    echo ""
    
    detect_package_manager
    log_info "Detected package manager: $PACKAGE_MANAGER"
    
    install_system_deps
    install_nodejs
    install_raylib
    setup_development_scripts
    
    echo ""
    verify_installation
    
    echo ""
    log_success "🎉 Setup completed successfully!"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. Restart your shell or run: source ~/.bashrc"
    echo "2. Run: make all (to build everything)"
    echo "3. Use: ./dev.sh (for development mode)"
    echo ""
}

main "$@"