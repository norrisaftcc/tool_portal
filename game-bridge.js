/**
 * PULSE Play Portal Game Bridge
 * 
 * This script provides communication between games and the portal.
 * Include this in your game HTML files to enable credit updates and achievements.
 */

// Game Bridge API
const GameBridge = {
    /**
     * Initialize the bridge
     * @param {Object} options - Configuration options
     */
    init: function(options = {}) {
        this.debugMode = options.debug || false;
        this.gameName = options.gameName || 'Unknown Game';
        this.log('Game Bridge initialized for ' + this.gameName);
        this.setupMessageListener();
    },
    
    /**
     * Set up listener for messages from the portal
     */
    setupMessageListener: function() {
        window.addEventListener('message', (event) => {
            // Validate message source (should be parent)
            if (event.source !== window.parent) return;
            
            try {
                const message = event.data;
                if (message.type === 'PORTAL_EVENT') {
                    this.handlePortalEvent(message);
                }
            } catch (error) {
                this.log('Error handling message: ' + error, true);
            }
        });
        
        // Notify portal that game is ready
        this.sendMessage({
            type: 'GAME_EVENT',
            action: 'GAME_READY',
            gameName: this.gameName
        });
    },
    
    /**
     * Handle events from the portal
     * @param {Object} message - Message from portal
     */
    handlePortalEvent: function(message) {
        this.log('Received portal event: ' + message.action);
        
        switch (message.action) {
            case 'CREDITS_UPDATED':
                // Portal confirms credits were updated
                if (typeof this.onCreditsUpdated === 'function') {
                    this.onCreditsUpdated(message.newBalance);
                }
                break;
                
            case 'ACHIEVEMENT_UNLOCKED':
                // Portal confirms achievement was unlocked
                if (typeof this.onAchievementUnlocked === 'function') {
                    this.onAchievementUnlocked(message.achievementId);
                }
                break;
                
            case 'PORTAL_READY':
                // Portal is ready to receive messages
                this.log('Portal is ready to receive messages');
                break;
        }
    },
    
    /**
     * Update player credits (win or lose)
     * @param {Number} amount - Amount to add (positive) or subtract (negative)
     * @param {String} reason - Reason for the update (optional)
     */
    updateCredits: function(amount, reason = '') {
        this.log('Updating credits: ' + amount + (reason ? ' (' + reason + ')' : ''));
        this.sendMessage({
            type: 'GAME_EVENT',
            action: 'UPDATE_CREDITS',
            amount: amount,
            reason: reason
        });
    },
    
    /**
     * Notify portal that player has completed a game
     * @param {Object} stats - Game statistics
     */
    completeGame: function(stats = {}) {
        this.log('Game completed');
        this.sendMessage({
            type: 'GAME_EVENT',
            action: 'GAME_COMPLETE',
            stats: stats
        });
    },
    
    /**
     * Unlock an achievement for the player
     * @param {String} achievementId - ID of the achievement to unlock
     */
    unlockAchievement: function(achievementId) {
        this.log('Unlocking achievement: ' + achievementId);
        this.sendMessage({
            type: 'GAME_EVENT',
            action: 'ACHIEVEMENT_UNLOCKED',
            achievementId: achievementId
        });
    },
    
    /**
     * Send a message to the portal
     * @param {Object} message - Message to send
     */
    sendMessage: function(message) {
        try {
            // Add timestamp to message
            message.timestamp = Date.now();
            message.gameName = this.gameName;
            
            window.parent.postMessage(message, '*');
            this.log('Sent message: ' + message.action);
        } catch (error) {
            this.log('Error sending message: ' + error, true);
        }
    },
    
    /**
     * Log messages to console in debug mode
     * @param {String} message - Message to log
     * @param {Boolean} isError - Whether this is an error message
     */
    log: function(message, isError = false) {
        if (this.debugMode) {
            const logMethod = isError ? console.error : console.log;
            logMethod('[GameBridge] ' + message);
        }
    },
    
    // Callback handlers (set these in your game)
    onCreditsUpdated: null,
    onAchievementUnlocked: null
};

// Auto-initialize in standalone mode
if (window === window.parent) {
    // Running outside the portal (standalone), use mock portal
    GameBridge.init({ debug: true, gameName: 'Standalone Mode' });
    console.log('Running in standalone mode - portal features are simulated');
}