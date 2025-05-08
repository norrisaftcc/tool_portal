# Makefile for compiling the raylib slot machine game to WebAssembly with Emscripten

# Configure paths
RAYLIB_PATH ?= ./raylib
GAME_NAME = raylib-slots
OUTPUT_DIR = ../..

# Emscripten compiler and flags
EMCC = emcc
EMSDK ?= /usr/local/emsdk  # Change this to your Emscripten SDK path

# Compiler flags
CFLAGS = -std=c++14 -Wall
INCLUDE_PATHS = -I$(RAYLIB_PATH)/src -I.
LDFLAGS = -L$(RAYLIB_PATH)/src

# Emscripten-specific flags
EM_FLAGS = -s ASYNCIFY=1 \
           -s USE_GLFW=3 \
           -s ALLOW_MEMORY_GROWTH=1 \
           -s TOTAL_MEMORY=67108864 \
           -s EXPORTED_FUNCTIONS="['_main', '_updateCredits', '_getCredits', '_forceWin']" \
           -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
           --preload-file images \
           --preload-file sounds \
           --shell-file shell.html

# Detect if Emscripten is available and set compilation mode
ifneq ($(shell which emcc),)
	# Emscripten is available, use it to compile to WASM
	CC = $(EMCC)
	CFLAGS += $(EM_FLAGS)
	OUTPUT_EXT = .html
else
	$(warning Emscripten compiler not found. The game will be compiled as a native binary.)
	# Fallback to native compilation
	ifeq ($(OS),Windows_NT)
		CC = g++
		OUTPUT_EXT = .exe
		LDFLAGS += -lraylib -lopengl32 -lgdi32 -lwinmm
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Darwin)
			CC = clang++
			OUTPUT_EXT = .app
			LDFLAGS += -lraylib -framework OpenGL -framework Cocoa
		else
			CC = g++
			OUTPUT_EXT = 
			LDFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
		endif
	endif
endif

# Source files
SRC = slotmachine.cpp
OUTPUT = $(OUTPUT_DIR)/$(GAME_NAME)$(OUTPUT_EXT)

# Default target
all: $(OUTPUT)

# Rule to build the game
$(OUTPUT): $(SRC) | prepare
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(SRC) -o $(OUTPUT) $(LDFLAGS)

# Prepare the environment
prepare:
	@mkdir -p $(OUTPUT_DIR)

# Clone and build Raylib (if needed)
raylib:
	@if [ ! -d "$(RAYLIB_PATH)" ]; then \
		git clone https://github.com/raysan5/raylib.git --branch 4.5.0 --depth 1; \
		cd $(RAYLIB_PATH)/src && $(MAKE) PLATFORM=PLATFORM_WEB -B; \
	fi

# Clean target
clean:
	rm -f $(OUTPUT)

# Help target
help:
	@echo "Available targets:"
	@echo "  all     : Build the game (default)"
	@echo "  raylib  : Clone and build Raylib (if needed)"
	@echo "  clean   : Remove the built game"

.PHONY: all prepare raylib clean help