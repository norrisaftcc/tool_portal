# Game Integration Guide for PULSE Play Portal

This guide explains how to properly integrate games into the PULSE Play casino games portal.

## Portal Architecture

The PULSE Play portal is a React-based application that loads and displays casino games. The portal provides:

- A centralized navigation system
- Game discovery features
- Authentication and player profile management
- Credits/coins system (future)

## Integration Methods

There are two ways to integrate games into the portal:

### 1. Iframe Integration (Simple)

The simplest integration method is through iframes. The portal loads the game HTML files in iframes, providing:

- Basic isolation between games
- Simple back navigation to the portal
- Support for existing standalone games

**Limitations:**
- Limited communication between game and portal
- Potential performance issues
- Cannot easily share state with the portal (e.g., player credits)

### 2. Component Integration (Recommended)

The recommended approach is to integrate games as React components, using the `GameContainer` component:

- Better performance
- Direct access to portal state
- Ability to share styling and resources
- Seamless integration with portal features

## Integration Steps

### For Iframe Integration:

1. Create your game as a standalone HTML file
2. Remove any "Back to Portal" buttons (navigation is handled by the portal)
3. Ensure your game works properly when loaded in an iframe
4. Add your game to the `GAMES` object in `index.html`

### For Component Integration:

1. Create your game logic as a module with init and cleanup functions
2. Use the `GameContainer` component in `game-container.js` to wrap your game
3. Update the portal to use your component instead of the iframe
4. Add your game to the `GAMES` object in `index.html`

## Game Structure Requirements

For proper integration, games should:

1. Not use their own navigation back to the portal
2. Be responsive and adapt to the container size
3. Use event listeners rather than inline `onclick` attributes
4. Avoid CSS conflicts with the portal
5. Support cleanup when game is closed

## Example: Converting Existing Games

To convert an existing standalone game for portal integration:

1. Extract the game logic into a module
2. Create init and cleanup functions
3. Remove any portal navigation
4. Ensure the game container has flexible dimensions
5. Test the game within the portal

## Handling Game State

The portal provides:

- Game selection and navigation
- Player profile and credits (future)

Your game should:

- Initialize with player data from the portal
- Update credits/stats through portal mechanisms
- Clean up resources when game is closed

## Testing Your Integration

To test your game integration:

1. Make sure `index.html` is serving correctly
2. Test that your game loads properly in the portal
3. Verify that navigation works (back to portal)
4. Check that game state is preserved correctly
5. Test on different screen sizes

## Troubleshooting

Common issues:

- Game doesn't load: Check file paths and scripts
- Styling issues: Look for CSS conflicts
- Performance problems: Optimize resource loading
- Navigation issues: Ensure portal handles all navigation

## Additional Resources

- React documentation: [https://reactjs.org/docs/getting-started.html](https://reactjs.org/docs/getting-started.html)
- PULSE Play development: See README_dev_plans.md