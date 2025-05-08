# Game Integration Guide for PULSE Play Portal

This guide explains how to integrate games into the PULSE Play casino games portal.

## Overview

The PULSE Play portal is built with React and uses a centralized game container system. Games can be integrated in two ways:

1. **Iframe Integration** (Simplest): Games are loaded as standalone HTML files in an iframe
2. **Component Integration** (Advanced): Games are built as React components that can directly interact with the portal

## Game Configuration

All games are defined in the `GAMES` object in `index.html`. To add a new game, add a new entry to this object:

```javascript
const GAMES = {
    // Existing games...
    
    yourGameId: {
        id: 'yourGameId',            // Unique identifier for the game
        name: 'Your Game Name',      // Display name
        path: 'your-game.html',      // Path to game HTML file or component
        category: 'slots',           // Game category (card, slots, table, etc.)
        hot: false,                  // Featured as "hot" game?
        multiplier: '5x',            // Multiplier for marketing
        playing: 100,                // Number of fake players
        available: true              // Is the game available or coming soon?
    }
};
```

## Option 1: Iframe Integration (Recommended for HTML/JS Games)

### Step 1: Create Your Game HTML File

Create your game as a standalone HTML file including all necessary styles and scripts. 

Example structure:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Your Game Name</title>
    <style>
        /* Your game styles */
    </style>
</head>
<body>
    <!-- Your game HTML -->
    
    <script>
        // Your game JavaScript
    </script>
</body>
</html>
```

### Step 2: Add Game to Configuration

Add your game to the `GAMES` object in `index.html` as shown above.

### Step 3: Test Integration

Test that your game loads properly in the portal by:
1. Clicking on your game card in the portal
2. Ensuring the game loads in the iframe
3. Testing that the "Back to Portal" button works

## Option 2: Component Integration (Advanced)

For more advanced integration, you can build your game as a React component. This allows for direct interaction with the portal's state management.

### Step 1: Create Your Game Component

Create a new JavaScript file for your game component:

```javascript
// game-component.js
const YourGame = ({ credits, onCreditChange }) => {
    // Game state and logic
    
    // Example of updating credits
    const winGame = () => {
        onCreditChange(prevCredits => prevCredits + 100);
    };
    
    return (
        <div className="your-game">
            {/* Game UI */}
            <div>Credits: {credits}</div>
            <button onClick={winGame}>Win Game</button>
        </div>
    );
};
```

### Step 2: Modify GameContainer Component

Update the `GameContainer` component in `index.html` to support component-based games:

```javascript
const GameContainer = ({ gameId, onClose, credits, onCreditChange }) => {
    // Existing code...
    
    // For component-based games
    if (game.component) {
        const GameComponent = game.component;
        return (
            <div className="game-container active">
                <button className="back-button" onClick={onClose}>
                    <ArrowLeft size={16} className="mr-2" /> Back to Portal
                </button>
                
                <GameComponent 
                    credits={credits}
                    onCreditChange={onCreditChange}
                />
            </div>
        );
    }
    
    // For iframe-based games (existing code)
    return (
        // Existing iframe implementation...
    );
};
```

## Best Practices

1. **Currency Management**: 
   - For iframe games, implement a postMessage API to communicate with the parent portal
   - For component games, use the provided `credits` and `onCreditChange` props

2. **Responsive Design**:
   - Games should be responsive and work well on both desktop and mobile
   - Test your games on various screen sizes

3. **Loading Performance**:
   - Minimize asset sizes
   - Use progressive loading for larger games
   - Provide loading indicators

4. **Error Handling**:
   - Implement proper error boundaries
   - Gracefully handle game failures

## Communication Protocol for Iframe Games

To communicate between the game iframe and the portal, use the following postMessage protocol:

```javascript
// From game to portal
window.parent.postMessage({
    type: 'GAME_EVENT',
    action: 'UPDATE_CREDITS',
    amount: 100  // Amount to add (positive) or subtract (negative)
}, '*');

// Additional actions
// action: 'GAME_COMPLETE' - Notify the portal the game is complete
// action: 'ACHIEVEMENT_UNLOCKED', achievementId: 'achievement1' - Unlock an achievement
```

## Troubleshooting

### Game Doesn't Load
- Check the console for errors
- Verify the path in the GAMES configuration is correct
- Ensure all game assets are properly referenced

### Credit Updates Not Working
- For iframe games, verify postMessage is working
- For component games, check that onCreditChange is being called

### Game Display Issues
- Check for CSS conflicts
- Verify the game is responsive
- Test on multiple browsers

## Example Games

See the existing games for reference:
- `blackjack-game.html` - Example of a card game
- `slots-html.html` - Example of a slot machine game