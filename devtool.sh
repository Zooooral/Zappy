#!/bin/bash

set -e

readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly ENV_FILE="${SCRIPT_DIR}/.env"
readonly LOG_DIR="${SCRIPT_DIR}/logs"
readonly PID_DIR="${SCRIPT_DIR}/.pids"

readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly CYAN='\033[0;36m'
readonly WHITE='\033[1;37m'
readonly BOLD='\033[1m'
readonly NC='\033[0m'

show_banner() {
  echo -e "${PURPLE}"
  cat << "EOF"
    ███████╗ █████╗ ██████╗ ██████╗ ██╗   ██╗
    ╚══███╔╝██╔══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝
      ███╔╝ ███████║██████╔╝██████╔╝ ╚████╔╝ 
     ███╔╝  ██╔══██║██╔═══╝ ██╔═══╝   ╚██╔╝  
    ███████╗██║  ██║██║     ██║        ██║   
    ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝        ╚═╝   
EOF
  echo -e "${CYAN}    🚀 Development CLI Tool v1.0${NC}"
  echo -e "${YELLOW}    Making Zappy development a breeze!${NC}\n"
}

init_directories() {
  mkdir -p "${LOG_DIR}" "${PID_DIR}"
}

load_env() {
  if [[ -f "${ENV_FILE}" ]]; then
    set -a
    source "${ENV_FILE}"
    set +a
    echo -e "${GREEN}✓${NC} Environment loaded from .env"
  else
    create_default_env
  fi
}

create_default_env() {
  echo -e "${YELLOW}⚠️${NC} No .env file found. Creating default configuration..."
  
  cat > "${ENV_FILE}" << 'EOF'
# Zappy Development Configuration

# Server Configuration
ZAPPY_PORT=4242
ZAPPY_WIDTH=20
ZAPPY_HEIGHT=20
ZAPPY_FREQ=100
ZAPPY_MAX_CLIENTS=10

# Team Configuration (space-separated)
ZAPPY_TEAMS="Alpha Beta Gamma Delta Epsilon Zeta"

# GUI Configuration
ZAPPY_GUI_HOST=localhost

# AI Configuration
ZAPPY_AI_COUNT=3
ZAPPY_AI_TEAM=Alpha
ZAPPY_AI_HOST=localhost
ZAPPY_AI_DELAY=2

# Development Settings
ZAPPY_LOG_LEVEL=info
ZAPPY_AUTO_RESTART=true
ZAPPY_BUILD_PARALLEL=true
EOF
  
  echo -e "${GREEN}✓${NC} Default .env created! You can customize it before running commands."
  echo -e "${CYAN}💡${NC} Edit .env to adjust ports, teams, map size, etc."
}

log_info() {
  echo -e "${CYAN}[INFO]${NC} ${1}"
}

log_success() {
  echo -e "${GREEN}[SUCCESS]${NC} ${1}"
}

log_error() {
  echo -e "${RED}[ERROR]${NC} ${1}"
}

log_warning() {
  echo -e "${YELLOW}[WARNING]${NC} ${1}"
}

save_pid() {
  local name="${1}"
  local pid="${2}"
  echo "${pid}" > "${PID_DIR}/${name}.pid"
}

get_pid() {
  local name="${1}"
  local pid_file="${PID_DIR}/${name}.pid"
  if [[ -f "${pid_file}" ]]; then
    cat "${pid_file}"
  fi
}

is_running() {
  local name="${1}"
  local pid
  pid=$(get_pid "${name}")
  if [[ -n "${pid}" ]] && kill -0 "${pid}" 2>/dev/null; then
    return 0
  else
    rm -f "${PID_DIR}/${name}.pid"
    return 1
  fi
}

kill_process() {
  local name="${1}"
  local pid
  pid=$(get_pid "${name}")
  if [[ -n "${pid}" ]]; then
    if kill -0 "${pid}" 2>/dev/null; then
      kill "${pid}" 2>/dev/null || kill -9 "${pid}" 2>/dev/null
      log_info "Stopped ${name} (PID: ${pid})"
    fi
    rm -f "${PID_DIR}/${name}.pid"
  fi
}

build_component() {
  local component="${1}"
  local log_file="${LOG_DIR}/build_${component}.log"
  
  log_info "Building ${component}..."
  
  if [[ "${ZAPPY_BUILD_PARALLEL}" == "true" ]]; then
    make "${component}" -j"$(nproc)" > "${log_file}" 2>&1 &
    local build_pid=$!
    save_pid "build_${component}" "${build_pid}"
    
    show_build_progress "${component}" "${build_pid}" "${log_file}"
  else
    if make "${component}" > "${log_file}" 2>&1; then
      log_success "${component} built successfully"
    else
      log_error "${component} build failed. Check ${log_file}"
      return 1
    fi
  fi
}

show_build_progress() {
  local component="${1}"
  local pid="${2}"
  local log_file="${3}"
  
  while kill -0 "${pid}" 2>/dev/null; do
    echo -n "."
    sleep 0.5
  done
  
  wait "${pid}"
  local exit_code=$?
  
  if [[ ${exit_code} -eq 0 ]]; then
    log_success "${component} built successfully"
  else
    log_error "${component} build failed. Check ${log_file}"
    return 1
  fi
}

build_all() {
  log_info "Building all components..."
  
  if [[ "${ZAPPY_BUILD_PARALLEL}" == "true" ]]; then
    build_component "server" &
    build_component "gui" &
    build_component "ai" &
    wait
  else
    build_component "server"
    build_component "gui" 
    build_component "ai"
  fi
  
  log_success "All components built!"
}

clean_build() {
  log_info "Cleaning build files..."
  make fclean > "${LOG_DIR}/clean.log" 2>&1
  log_success "Build files cleaned"
}

rebuild_all() {
  log_info "Rebuilding all components..."
  make re > "${LOG_DIR}/rebuild.log" 2>&1 &
  local rebuild_pid=$!
  
  while kill -0 "${rebuild_pid}" 2>/dev/null; do
    echo -n "🔨"
    sleep 1
  done
  
  wait "${rebuild_pid}"
  log_success "All components rebuilt!"
}

start_server() {
  if is_running "server"; then
    log_warning "Server is already running"
    return 0
  fi
  
  log_info "Starting Zappy server..."
  
  local cmd="./zappy_server -p ${ZAPPY_PORT} -x ${ZAPPY_WIDTH} -y ${ZAPPY_HEIGHT} -c ${ZAPPY_MAX_CLIENTS} -f ${ZAPPY_FREQ} -n ${ZAPPY_TEAMS}"
  
  ${cmd} > "${LOG_DIR}/server.log" 2>&1 &
  local server_pid=$!
  save_pid "server" "${server_pid}"
  
  sleep 2
  if is_running "server"; then
    log_success "Server started (PID: ${server_pid}, Port: ${ZAPPY_PORT})"
    log_info "Map: ${ZAPPY_WIDTH}x${ZAPPY_HEIGHT}, Teams: ${ZAPPY_TEAMS}"
  else
    log_error "Failed to start server. Check ${LOG_DIR}/server.log"
    return 1
  fi
}

start_server_seed() {
  if is_running "server"; then
    log_warning "Server is already running"
    return 0
  fi
  
  log_info "Starting Zappy server in seed mode..."
  
  ./zappy_server --seed -p "${ZAPPY_PORT}" > "${LOG_DIR}/server_seed.log" 2>&1 &
  local server_pid=$!
  save_pid "server" "${server_pid}"
  
  sleep 2
  if is_running "server"; then
    log_success "Server started in seed mode (PID: ${server_pid}, Port: ${ZAPPY_PORT})"
  else
    log_error "Failed to start server. Check ${LOG_DIR}/server_seed.log"
    return 1
  fi
}

stop_server() {
  if is_running "server"; then
    kill_process "server"
    log_success "Server stopped"
  else
    log_warning "Server is not running"
  fi
}

start_gui() {
  if is_running "gui"; then
    log_warning "GUI is already running"
    return 0
  fi
  
  log_info "Starting Zappy GUI..."
  
  ./zappy_gui -p "${ZAPPY_PORT}" -h "${ZAPPY_GUI_HOST}" > "${LOG_DIR}/gui.log" 2>&1 &
  local gui_pid=$!
  save_pid "gui" "${gui_pid}"
  
  sleep 1
  if is_running "gui"; then
    log_success "GUI started (PID: ${gui_pid})"
  else
    log_error "Failed to start GUI. Check ${LOG_DIR}/gui.log"
    return 1
  fi
}

stop_gui() {
  if is_running "gui"; then
    kill_process "gui"
    log_success "GUI stopped"
  else
    log_warning "GUI is not running (checking for any remaining GUI processes...)"
    if pkill -f "zappy_gui" 2>/dev/null; then
      sleep 1
      if pgrep -f "zappy_gui" >/dev/null 2>&1; then
        log_warning "GUI didn't respond to SIGTERM, force killing..."
        pkill -9 -f "zappy_gui" 2>/dev/null
      fi
      log_success "Remaining GUI processes terminated"
    else
      log_warning "No GUI processes found"
    fi
  fi
}

start_ai() {
  local team="${1:-${ZAPPY_AI_TEAM}}"
  local count="${2:-${ZAPPY_AI_COUNT}}"
  
  log_info "Starting ${count} AI clients for team '${team}'..."
  
  for ((i=1; i<=count; i++)); do
    local ai_name="ai_${team}_${i}"
    
    if is_running "${ai_name}"; then
      log_warning "AI ${ai_name} is already running"
      continue
    fi
    
    ./zappy_ai -p "${ZAPPY_PORT}" -n "${team}" -h "${ZAPPY_AI_HOST}" \
      > "${LOG_DIR}/${ai_name}.log" 2>&1 &
    local ai_pid=$!
    save_pid "${ai_name}" "${ai_pid}"
    
    log_success "AI ${ai_name} started (PID: ${ai_pid})"
    
    if [[ "${ZAPPY_AI_DELAY}" -gt 0 ]]; then
      sleep "${ZAPPY_AI_DELAY}"
    fi
  done
}

start_ai_all_teams() {
  local teams_array=(${ZAPPY_TEAMS})
  local ai_per_team="${1:-2}"
  
  for team in "${teams_array[@]}"; do
    start_ai "${team}" "${ai_per_team}"
  done
}

stop_ai() {
  local pattern="${1:-ai_}"
  local stopped_count=0
  
  for pid_file in "${PID_DIR}"/${pattern}*.pid; do
    if [[ -f "${pid_file}" ]]; then
      local name
      name=$(basename "${pid_file}" .pid)
      if is_running "${name}"; then
        kill_process "${name}"
        ((stopped_count++))
      fi
    fi
  done
  
  if [[ ${stopped_count} -gt 0 ]]; then
    log_success "Stopped ${stopped_count} AI client(s)"
  else
    log_warning "No AI clients were running"
  fi
}

dev_setup() {
  log_info "Setting up development environment..."
  
  build_all
  start_server
  sleep 3
  start_gui
  sleep 2
  start_ai
  
  log_success "Development environment ready!"
}

dev_restart() {
  log_info "Restarting development environment..."
  
  stop_all
  rebuild_all
  sleep 2
  dev_setup
}

quick_test() {
  local team="${1:-Alpha}"
  
  log_info "Quick test setup for team '${team}'..."
  
  if ! is_running "server"; then
    start_server_seed
    sleep 2
  fi
  
  start_ai "${team}" 1
  
  if ! is_running "gui"; then
    start_gui
  fi
  
  log_success "Quick test ready! AI connected to server."
}

stop_all() {
  log_info "Stopping all processes..."
  
  log_info "Terminating AI clients..."
  if pkill -f "zappy_ai" 2>/dev/null; then
    log_success "AI clients terminated"
  else
    log_warning "No AI clients found running"
  fi
  
  log_info "Terminating GUI..."
  if pkill -f "zappy_gui" 2>/dev/null; then
    sleep 1
    if pgrep -f "zappy_gui" >/dev/null 2>&1; then
      log_warning "GUI didn't respond to SIGTERM, force killing..."
      pkill -9 -f "zappy_gui" 2>/dev/null
    fi
    log_success "GUI terminated"
  else
    log_warning "No GUI found running"
  fi
  
  log_info "Terminating server..."
  if pkill -f "zappy_server" 2>/dev/null; then
    sleep 1
    if pgrep -f "zappy_server" >/dev/null 2>&1; then
      log_warning "Server didn't respond to SIGTERM, force killing..."
      pkill -9 -f "zappy_server" 2>/dev/null
    fi
    log_success "Server terminated"
  else
    log_warning "No server found running"
  fi
  
  sleep 1
  
  log_info "Cleaning PID directory..."
  rm -f "${PID_DIR}"/*.pid
  
  log_success "All processes stopped and PID directory cleaned"
}

config() {
  case "${1:-show}" in
    "show")
      echo -e "\n${BOLD}⚙️  Current Configuration${NC}\n"
      if [[ -f "${ENV_FILE}" ]]; then
        cat "${ENV_FILE}"
      else
        log_error "No .env file found"
      fi
      ;;
    "edit")
      ${EDITOR:-nano} "${ENV_FILE}"
      log_info "Configuration updated. Restart processes to apply changes."
      ;;
    "reset")
      rm -f "${ENV_FILE}"
      create_default_env
      ;;
    *)
      echo "Usage: config [show|edit|reset]"
      ;;
  esac
}

help() {
  echo -e "\n${BOLD}🚀 Zappy Development CLI Tool${NC}\n"
  
  echo -e "${CYAN}📦 Build Commands:${NC}"
  echo -e "  build [server|gui|ai|all]  Build specific component or all"
  echo -e "  clean                      Clean build files"
  echo -e "  rebuild                    Clean and build all"
  
  echo -e "\n${CYAN}🖥️  Server Commands:${NC}"
  echo -e "  server start               Start the server"
  echo -e "  server stop                Stop the server"
  echo -e "  server seed                Start server in seed mode"
  
  echo -e "\n${CYAN}🎮 GUI Commands:${NC}"
  echo -e "  gui start                  Start the GUI"
  echo -e "  gui stop                   Stop the GUI"
  
  echo -e "\n${CYAN}🤖 AI Commands:${NC}"
  echo -e "  ai start [team] [count]    Start AI clients"
  echo -e "  ai stop [pattern]          Stop AI clients"
  echo -e "  ai all [count]             Start AIs for all teams"
  
  echo -e "\n${CYAN}🔄 Workflow Commands:${NC}"
  echo -e "  dev                        Full development setup"
  echo -e "  restart                    Restart everything"
  echo -e "  test [team]                Quick test setup"
  echo -e "  stop                       Stop all processes (force kill if needed)"
  
  echo -e "\n${CYAN}⚙️  Configuration Commands:${NC}"
  echo -e "  config [show|edit|reset]   Manage configuration"
  
  echo -e "\n${CYAN}💡 Examples:${NC}"
  echo -e "  ${0} dev                     Start full development environment"
  echo -e "  ${0} test Alpha              Quick test with Alpha team"
  echo -e "  ${0} ai start Beta 5         Start 5 AI clients for Beta team"
  echo -e "  ${0} stop                    Stop all processes (force kill if needed)"
}

main() {
  init_directories
  load_env
  
  if [[ $# -eq 0 ]]; then
    show_banner
    help
    return 0
  fi
  
  case "${1}" in
    "build")
      case "${2:-all}" in
        "server"|"gui"|"ai") build_component "${2}" ;;
        "all") build_all ;;
        *) log_error "Invalid component: ${2}" ;;
      esac
      ;;
    "clean") clean_build ;;
    "rebuild") rebuild_all ;;
    
    "server")
      case "${2}" in
        "start") start_server ;;
        "stop") stop_server ;;
        "seed") start_server_seed ;;
        *) log_error "Usage: server [start|stop|seed]" ;;
      esac
      ;;
    
    "gui")
      case "${2}" in
        "start") start_gui ;;
        "stop") stop_gui ;;
        *) log_error "Usage: gui [start|stop]" ;;
      esac
      ;;
    
    "ai")
      case "${2}" in
        "start") start_ai "${3}" "${4}" ;;
        "stop") stop_ai "${3}" ;;
        "all") start_ai_all_teams "${3}" ;;
        *) log_error "Usage: ai [start|stop|all]" ;;
      esac
      ;;
    
    "dev") dev_setup ;;
    "restart") dev_restart ;;
    "test") quick_test "${2}" ;;
    "stop") stop_all ;;
    
    "config") config "${2}" ;;
    
    "help"|"-h"|"--help") help ;;
    
    *)
      log_error "Unknown command: ${1}"
      echo "Run '${0} help' for usage information"
      exit 1
      ;;
  esac
}

main "$@"