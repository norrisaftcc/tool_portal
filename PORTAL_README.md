# PULSE Play - Casual Games Portal

## Overview

PULSE Play is a mobile-first casino games portal targeting males 18-25 with simple, rewarding gameplay experiences. The portal integrates multiple casual games with a shared virtual currency system.

## Features

- **Unified Portal Interface**: A central hub for accessing all games
- **In-Portal Game Playing**: Games load within the portal, no page navigation required
- **Shared Virtual Currency**: Credits are maintained across all games
- **Achievement System**: In-game achievements are tracked portal-wide
- **Responsive Design**: Works on mobile and desktop devices

## Available Games

1. **Blackjack Blitz**: Fast-paced blackjack with betting system
2. **Golden Fortune**: Slot machine with multiple paylines and bonus features

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

Games are integrated with the portal using:
- Iframe-based loading
- PostMessage API for communication
- GameBridge.js for standardized messaging

## Game Integration

To integrate a new game, follow these steps:

1. Create your game as a standalone HTML file
2. Include the GameBridge.js script in your game
3. Initialize GameBridge in your game
4. Update the GAMES configuration in index.html
5. Test your integration

For detailed integration instructions, see [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md).

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