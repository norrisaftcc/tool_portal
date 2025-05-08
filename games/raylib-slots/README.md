# Raylib Slot Machine for PULSE Play Portal

This directory contains a WebAssembly-based slot machine game implemented using raylib and integrated with the PULSE Play portal.

## Prerequisites

To build this game, you'll need:

1. **Emscripten SDK** - For compiling the C++ code to WebAssembly.
   ```bash
   # Clone the Emscripten SDK
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   
   # Install and activate the latest SDK
   ./emsdk install latest
   ./emsdk activate latest
   
   # Add to your PATH (you'll need to do this each time you open a new terminal)
   source ./emsdk_env.sh
   ```

2. **Raylib source code** - The WebAssembly build needs to compile raylib from source.
   ```bash
   # From within the raylib-slots directory
   git clone https://github.com/raysan5/raylib.git --branch 4.5.0 --depth 1
   ```

## Building the Game

1. Navigate to the raylib-slots directory:
   ```bash
   cd games/raylib-slots
   ```

2. Build raylib for WebAssembly (if you haven't already):
   ```bash
   cd raylib/src
   make PLATFORM=PLATFORM_WEB -B
   cd ../..
   ```

3. Build the game:
   ```bash
   make
   ```

This will create the WebAssembly files in the project root directory:
- `raylib-slots.html` - The main HTML file
- `raylib-slots.js` - JavaScript glue code 
- `raylib-slots.wasm` - The WebAssembly binary
- `raylib-slots.data` - The preloaded assets (images and sounds)

## Manual Build (Alternative)

If you encounter issues with the Makefile, you can build the game manually:

```bash
emcc slotmachine.cpp -o ../../raylib-slots.html \
    -I./raylib/src \
    -L./raylib/src \
    -lraylib \
    -s USE_GLFW=3 \
    -s ASYNCIFY=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s TOTAL_MEMORY=67108864 \
    -s EXPORTED_FUNCTIONS="['_main', '_updateCredits', '_getCredits', '_forceWin']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
    --preload-file images \
    --preload-file sounds \
    --shell-file shell.html
```

## Game Assets

The game requires the following assets:

1. Images:
   - Fruit icons: `cherry.png`, `watermelon.png`, `bar.png`, `bell.png`, `coconut.png`, `orange.png`, `pear.png`, `seven.png`
   - Button textures: `startRelaxedButton.png`, `startPressedButton.png`, `exitRelaxedButton.png`, `exitPressedButton.png`, etc.
   - Slot machine overlay: `pixel_art_slot_machine_-_machine_frame2.png`

2. Sounds:
   - `spin.mp3` - Played when the reels spin
   - `win.ogg` - Played on a winning combination
   - `buttonClick.ogg` - Played when clicking buttons

## Portal Integration

The game integrates with the PULSE Play portal through the following mechanisms:

1. **Credits System** - The game syncs credits with the portal
2. **Event Communication** - Events like wins, bets, and game state changes are sent to the portal
3. **Portal Interface** - The portal can control the game (force wins, update credits, etc.)

## WebAssembly Exported Functions

The game exports several JavaScript-callable functions:

- `_updateCredits(amount)` - Updates the player's credit balance
- `_getCredits()` - Returns the current credit balance
- `_forceWin()` - Forces a winning combination (for testing)

## Troubleshooting

- **Blank Screen**: Check the browser console for errors. WebAssembly may fail to load if CORS headers are missing.
- **No Sound**: Web browsers require user interaction before playing sound. Make sure to click on the game first.
- **Performance Issues**: Try reducing the `TOTAL_MEMORY` value if the game is slow on older devices.

## Credits

- Raylib - [raysan5/raylib](https://github.com/raysan5/raylib)
- Original slot machine code - Based on the SlotmachineRaylib project