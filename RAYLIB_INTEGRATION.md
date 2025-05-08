# Integrating Raylib Slot Machine with the Portal

## Overview

After examining the raylib-based slot machine game, I've assessed the feasibility of integrating it into the PULSE Play portal using Emscripten. This document outlines the approach, challenges, and implementation plan.

## Feasibility Assessment

### Positive Factors

1. **Emscripten Support in Raylib**: Raylib officially supports Emscripten compilation to WebAssembly/HTML5, as seen in the Makefile (lines 226-249).

2. **Clean Architecture**: The slot machine code has a clean structure with clear separation between rendering, game logic, and input handling.

3. **No External Dependencies**: The game only depends on Raylib and standard C++ libraries, making it easier to compile to WebAssembly.

4. **Available Assets**: All required images and sounds are included in the project.

### Challenges

1. **Game Loop Adaptation**: The game uses a traditional game loop not optimized for web. It needs to be adapted using `emscripten_set_main_loop()` (as mentioned in the Makefile line 171).

2. **Audio System Differences**: Web browser audio restrictions might require changes to how sounds are loaded and played.

3. **Memory Management**: WebAssembly has different memory constraints than desktop applications, requiring optimization.

4. **Portal Integration**: Communication between WebAssembly and JavaScript requires custom bindings.

## Implementation Plan

### Step 1: Compile with Emscripten

```bash
# Install Emscripten if not already installed
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Navigate to the SlotmachineRaylib directory
cd /Users/norrisa/Documents/dev/github/tool_portal/SlotmachineRaylib

# Compile the game to WebAssembly
emcc main.cpp -o web/slotmachine.html \
    -I/path/to/raylib/src \
    -L/path/to/raylib/src \
    -lraylib \
    -s USE_GLFW=3 \
    -s ASYNCIFY=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s TOTAL_MEMORY=67108864 \
    --preload-file images/ \
    --preload-file sounds/ \
    -s EXPORTED_FUNCTIONS="['_main', '_update_credits']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"
```

### Step 2: Modify the Code for Web Compatibility

The main code needs modification to work with Emscripten:

```cpp
#include <emscripten.h>
#include <emscripten/html5.h>

// Game state that must persist between frames
typedef struct GameState {
    vector<vector<int>> grid;
    vector<Texture2D> fruitTextures;
    bool startGame;
    int playerBalance;
    string matchMessage;
    // ... other necessary state variables
} GameState;

GameState gameState;

// Main game loop function for emscripten
void main_loop() {
    // Handle one frame of the game
    // Move logic from the while loop in main() to here
    
    // Input handling
    if (IsKeyPressed(KEY_E)) {
        // Spin logic...
    }
    
    // Rendering
    BeginDrawing();
    ClearBackground(DARKGREEN);
    // Draw game elements...
    EndDrawing();
}

// JavaScript callable function to update credits
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void update_credits(int amount) {
        gameState.playerBalance += amount;
    }
}

int main() {
    // Initialize once
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slot Machine");
    InitAudioDevice();
    // Load resources...
    
    // Initialize game state
    gameState.grid = GenerateRandomGrid(fruitTextures.size());
    gameState.startGame = true;
    gameState.playerBalance = 100;
    gameState.matchMessage = "";
    
    // Start the main loop
    emscripten_set_main_loop(main_loop, 0, 1);
    
    // These will never be called in web (emscripten handles cleanup)
    CloseWindow();
    CloseAudioDevice();
    
    return 0;
}
```

### Step 3: Create JavaScript Bridge for Portal Integration

Create a JavaScript file to interface between the WebAssembly game and the portal:

```javascript
// raylib-bridge.js
class RaylibSlotMachine {
    constructor(containerId, portalInterface) {
        this.container = document.getElementById(containerId);
        this.portalInterface = portalInterface;
        this.credits = 100;
        this.gameLoaded = false;
        
        // Create iframe to contain the Emscripten output
        this.iframe = document.createElement('iframe');
        this.iframe.style.width = '100%';
        this.iframe.style.height = '100%';
        this.iframe.style.border = 'none';
        this.container.appendChild(this.iframe);
        
        // Load the WebAssembly game
        this.iframe.src = 'games/slotmachine.html';
        
        // Set up message handling for communication
        window.addEventListener('message', this.handleMessage.bind(this));
        
        // Initialize when game is loaded
        this.iframe.onload = () => {
            this.gameLoaded = true;
            this.sendMessage({type: 'INIT', credits: this.credits});
        };
    }
    
    handleMessage(event) {
        if (!event.data || !event.data.type) return;
        
        switch(event.data.type) {
            case 'WIN':
                const amount = event.data.amount || 0;
                this.credits += amount;
                this.portalInterface.updateCredits(amount, 'raylib slots win');
                break;
                
            case 'BET':
                const betAmount = event.data.amount || 0;
                this.credits -= betAmount;
                this.portalInterface.updateCredits(-betAmount, 'raylib slots bet');
                break;
                
            case 'GAME_READY':
                console.log('Raylib game is ready');
                break;
        }
    }
    
    sendMessage(msg) {
        if (!this.gameLoaded) return;
        this.iframe.contentWindow.postMessage(msg, '*');
    }
    
    updateCredits(newAmount) {
        this.credits = newAmount;
        if (this.gameLoaded) {
            const updateCredits = this.iframe.contentWindow.Module.cwrap('update_credits', null, ['number']);
            updateCredits(newAmount);
        }
    }
    
    cleanup() {
        window.removeEventListener('message', this.handleMessage.bind(this));
        this.container.removeChild(this.iframe);
    }
}
```

### Step 4: Integrate with the Portal

Update the GAMES configuration in index.html to include the raylib game:

```javascript
const GAMES = {
    // Existing games...
    
    raylib_slots: {
        id: 'raylib_slots',
        name: 'Raylib Slots',
        path: 'games/slotmachine.html',
        category: 'slots',
        hot: true,
        multiplier: '25x',
        playing: 210,
        available: true,
        type: 'wasm' // Indicate this is a WebAssembly game
    }
};
```

Modify the `GameContainer` component to handle WebAssembly games:

```javascript
const GameContainer = ({ gameId, onClose, credits, onCreditChange }) => {
    // Existing state...
    const game = GAMES[gameId];
    const containerRef = React.useRef(null);
    const gameInstanceRef = React.useRef(null);
    
    React.useEffect(() => {
        if (game.type === 'wasm') {
            // Create portal interface for the game
            const portalInterface = {
                updateCredits: (amount, reason) => {
                    onCreditChange(prevCredits => {
                        const newBalance = Math.max(0, prevCredits + amount);
                        return newBalance;
                    });
                }
            };
            
            // Initialize WebAssembly game
            gameInstanceRef.current = new RaylibSlotMachine('game-container', portalInterface);
            
            // Cleanup on unmount
            return () => {
                if (gameInstanceRef.current) {
                    gameInstanceRef.current.cleanup();
                    gameInstanceRef.current = null;
                }
            };
        }
    }, [game, onCreditChange]);
    
    // Update game credits when portal credits change
    React.useEffect(() => {
        if (game.type === 'wasm' && gameInstanceRef.current) {
            gameInstanceRef.current.updateCredits(credits);
        }
    }, [credits, game]);
    
    if (game.type === 'wasm') {
        return (
            <div className="game-container active">
                <button className="back-button" onClick={onClose}>
                    <ArrowLeft size={16} className="mr-2" /> Back to Portal
                </button>
                
                <div id="game-container" ref={containerRef} style={{width: '100%', height: '100%'}}></div>
            </div>
        );
    }
    
    // Existing iframe implementation for regular games...
};
```

## Technical Challenges and Solutions

### 1. Game Loop Adaptation

**Challenge**: Raylib's game loop needs to be adapted for the web environment.

**Solution**: Use Emscripten's `ASYNCIFY` flag and replace the while loop with `emscripten_set_main_loop()` to properly yield control back to the browser.

### 2. Audio Playback

**Challenge**: Browser audio requires user interaction before playback.

**Solution**: Defer audio initialization until the first user interaction, and provide fallback options when audio can't be played.

### 3. Asset Loading

**Challenge**: Assets need to be preloaded and packaged with the WebAssembly module.

**Solution**: Use Emscripten's `--preload-file` option to embed the assets and make them accessible to the code.

### 4. Communication with Portal

**Challenge**: WebAssembly code can't directly access JavaScript variables.

**Solution**: Use exported C functions and JavaScript wrapper functions to create a bidirectional communication channel.

## Performance Considerations

1. **File Size**: The compiled WebAssembly module will be 5-10MB with assets, so implement progressive loading.

2. **Memory Usage**: Optimize texture sizes and limit unnecessary allocations to reduce memory footprint.

3. **Load Time**: Implement a loading screen and consider code splitting to improve initial load time.

## Conclusion

Integrating the raylib-based slot machine with the PULSE Play portal is feasible using Emscripten. The implementation will require modifications to the game code and the development of a JavaScript bridge layer, but the end result will be a seamlessly integrated WebAssembly-powered game within the portal.

The recommended approach is to start with a proof-of-concept that compiles the game to WebAssembly and loads it in a simple HTML page, then progressively integrate it with the portal's systems.