# PULSE Play - Casual Games Portal

## Overview

PULSE Play is a mobile-first casino games portal targeting males 18-25 with simple, rewarding gameplay experiences. The portal integrates multiple casual games with a shared virtual currency system.

## Features

- **Unified Portal Interface**: A central hub for accessing all games
- **In-Portal Game Playing**: Games load within the portal, no page navigation required
- **Shared Virtual Currency**: Credits are maintained across all games
- **Achievement System**: In-game achievements are tracked portal-wide
- **Responsive Design**: Works on mobile and desktop devices
- **Multiple Game Technologies**: Support for both HTML/JS and WebAssembly games

## Available Games

1. **Blackjack Blitz**: Fast-paced blackjack with betting system (HTML/JS)
2. **Golden Fortune**: Slot machine with multiple paylines and bonus features (HTML/JS)
3. **Fruity Slots**: High-performance slot machine with realistic physics (WebAssembly/raylib)

## Getting Started

1. Open `index.html` in your browser to launch the portal
2. Click on any available game to start playing
3. Use the "Back to Portal" button to return to the main portal
4. Your credits are shared across all games

## Technical Implementation

### Portal Architecture

The portal is built using:
- React (loaded via CDN)
- Tailwind CSS (loaded via CDN)
- Lucide icons

### Game Integration

Games are integrated with the portal using one of two methods:

#### HTML/JavaScript Games
- Iframe-based loading
- PostMessage API for communication
- GameBridge.js for standardized messaging

#### WebAssembly Games
- Compiled C++ code using Emscripten
- Custom container component instead of iframes
- RaylibBridge.js for WebAssembly/JavaScript interop
- Event-based communication system

## Game Integration

### Adding an HTML/JavaScript Game

1. Create your game as a standalone HTML file
2. Include the GameBridge.js script in your game
3. Initialize GameBridge in your game
4. Update the GAMES configuration in index.html with type: 'iframe'
5. Test your integration

### Adding a WebAssembly Game

1. Create your C/C++ game (or adapt an existing one)
2. Modify the code to use Emscripten's APIs for browser integration
3. Set up a Makefile for compilation
4. Compile the game to WebAssembly using Emscripten
5. Update the GAMES configuration in index.html with type: 'wasm'
6. Test your integration

For detailed integration instructions, see:
- [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) - General integration guide
- [RAYLIB_INTEGRATION.md](RAYLIB_INTEGRATION.md) - WebAssembly-specific guide

## Communication Protocol

Games communicate with the portal using a standardized protocol:

- **Game to Portal**:
  - `UPDATE_CREDITS`: Update player credits
  - `GAME_COMPLETE`: Notify game completion
  - `ACHIEVEMENT_UNLOCKED`: Unlock an achievement

- **Portal to Game**:
  - `CREDITS_UPDATED`: Confirm credit update
  - `PORTAL_READY`: Portal is ready to receive messages

## Development Roadmap

For future development plans, see [TODO.md](TODO.md).

## Credits

Developed by Team Nexus:
- Alex Chen: Frontend Lead & UX Design
- Taylor Rodriguez: Backend Development & Game Integration
- Jordan Kim: Game Development & Graphics
- Morgan Patel: Database Architecture & User Analytics