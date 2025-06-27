##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Makefile for Zappy project
##

############
## COLORS ##
############

GREEN        = 	\033[0;32m
RESET        = 	\033[0m
BLUE         = 	\033[94m
RED          = 	\033[91m

###########
## BUILD ##
###########

CC           =  gcc
CPP          =  g++
NPM          =  npm
GUI_NAME     =  zappy_gui
SERVER_NAME  =  zappy_server
AI_NAME      =  zappy_ai

CPPFLAGS     =  -Wall -Wextra -std=c++17 -iquote ./include -iquote ./src
GUI_LIBS     =  -lraylib
CFLAGS       =  -Wall -Wextra -std=c11 -Wno-multichar -D_GNU_SOURCE -O2
LDFLAGS	     =  -lm
MAKEFLAGS    += -j$(shell expr $(shell nproc) - 2)

BUILD_DIR    =  ./build
SRC_DIR      =  ./src
INCLUDE_DIR  =  ./include
DOCS_DIR     =  ./documentation

SERVER_SRC_DIR = $(SRC_DIR)/server
SERVER_BUILD_DIR = $(BUILD_DIR)/server

GUI_SRC_DIR = $(SRC_DIR)/gui
GUI_BUILD_DIR = $(BUILD_DIR)/gui

AI_SRC_DIR = $(SRC_DIR)/ai

NODE_MODULES = $(DOCS_DIR)/node_modules
DOCS_BUILD   = $(DOCS_DIR)/build

all: server gui ai docs-build

assets:
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Copying assets...$(RESET)\n"
	@for file in assets/environment/*.obj_tmp; do \
		if [ -f "$file" ]; then \
			cp "$file" "${file%.obj_tmp}.obj"; \
		fi; \
	done
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Assets copied successfully$(RESET)\n"

server: $(SERVER_OBJS)
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Linking server...$(RESET)\n"
	@$(CC) -o $(SERVER_NAME) $(SERVER_OBJS) $(LDFLAGS)
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Server built successfully$(RESET)\n"

gui: $(GUI_OBJS)
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Linking GUI...$(RESET)\n"
	@$(CPP) -o $(GUI_NAME) $(GUI_OBJS) $(GUI_LIBS)
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)GUI built successfully$(RESET)\n"

ai:
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Creating AI executable...$(RESET)\n"
	@cd $(AI_SRC_DIR) && $(NPM) install
	@echo '#!/bin/bash' > $(AI_NAME)
	@echo 'cd $(shell pwd)/$(AI_SRC_DIR) && node ai-zappy "$@"' >> $(AI_NAME)
	@chmod +x $(AI_NAME)
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)AI executable created successfully$(RESET)\n"

docs-install: $(NODE_MODULES)

$(NODE_MODULES):
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Installing documentation dependencies...$(RESET)\n"
	@cd $(DOCS_DIR) && $(NPM) install

docs-dev: docs-install
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Starting documentation development server...$(RESET)\n"
	@cd $(DOCS_DIR) && $(NPM) start

docs-build: docs-install
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Building documentation...$(RESET)\n"
	@cd $(DOCS_DIR) && $(NPM) run build

docs-serve: docs-build
	@printf "$(GREEN)[OK]$(RESET) $(BLUE)Serving built documentation...$(RESET)\n"
	@cd $(DOCS_DIR) && $(NPM) run serve

docs-clean:
	@printf "$(RED)[CLEANING]$(RESET) $(BLUE)Cleaning documentation...$(RESET)\n"
	@$(RM) -r $(NODE_MODULES) $(DOCS_BUILD)

clean:
	@printf "$(RED)[CLEANING]$(RESET) $(BLUE)Removing obj files...$(RESET)\n"
	@$(RM) -r $(BUILD_DIR)
	@printf "$(RED)[CLEANING]$(RESET) $(BLUE)Removing AI build files...$(RESET)\n"
	@$(RM) -r $(AI_SRC_DIR)/dist $(AI_SRC_DIR)/node_modules

fclean: clean docs-clean
	@printf "$(RED)[CLEANING]$(RESET) $(BLUE)Removing executables...$(RESET)\n"
	@$(RM) $(SERVER_NAME) $(GUI_NAME) $(AI_NAME)
	@printf "$(RED)[CLEANING]$(RESET) $(BLUE)Removing copied assets...$(RESET)\n"
	@$(RM) assets/environment/*.obj
	@$(RM) -r $(AI_SRC_DIR)/dist $(AI_SRC_DIR)/log

re: fclean
	@$(MAKE) all

debug: CFLAGS += -g3 -DDEBUG
debug: CPPFLAGS += -g3 -DDEBUG
debug: re

.PHONY: all server gui ai assets docs-install docs-dev docs-build docs-serve docs-clean clean fclean re debug