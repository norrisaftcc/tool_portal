/**
 * Bridge for raylib WebAssembly games to the PULSE Play portal
 */

class RaylibBridge {
    /**
     * Initialize the bridge
     * @param {string} containerId - ID of the container element for the game
     * @param {Object} portalInterface - Interface to the portal
     * @param {Function} portalInterface.updateCredits - Function to update credits in the portal
     * @param {Function} portalInterface.showMessage - Function to show a message in the portal
     * @param {number} initialCredits - Initial credits value
     */
    constructor(containerId, portalInterface, initialCredits = 100) {
        this.container = document.getElementById(containerId);
        this.portalInterface = portalInterface;
        this.credits = initialCredits;
        this.gameLoaded = false;
        this.messageHandlerBound = this.handleMessage.bind(this);
        
        // Create iframe to contain the WebAssembly game
        this.iframe = document.createElement('iframe');
        this.iframe.style.width = '100%';
        this.iframe.style.height = '100%';
        this.iframe.style.border = 'none';
        this.container.appendChild(this.iframe);
        
        // Set up message handling for communication
        window.addEventListener('message', this.messageHandlerBound);
        
        // Load status
        this.gameInitialized = false;
        
        // Add loader element
        this.createLoader();
    }
    
    /**
     * Create a loading indicator
     */
    createLoader() {
        this.loader = document.createElement('div');
        this.loader.className = 'raylib-loader';
        this.loader.innerHTML = `
            <div class="spinner"></div>
            <div class="loading-text">Loading Game...</div>
        `;
        
        // Add styles
        const style = document.createElement('style');
        style.textContent = `
            .raylib-loader {
                position: absolute;
                top: 0;
                left: 0;
                width: 100%;
                height: 100%;
                display: flex;
                flex-direction: column;
                align-items: center;
                justify-content: center;
                background: rgba(0, 0, 0, 0.8);
                color: #FFD700;
                z-index: 10;
            }
            .raylib-loader .spinner {
                width: 50px;
                height: 50px;
                border: 5px solid rgba(255, 215, 0, 0.3);
                border-radius: 50%;
                border-top-color: #FFD700;
                animation: raylib-spin 1s infinite linear;
                margin-bottom: 20px;
            }
            .raylib-loader .loading-text {
                font-size: 18px;
                font-weight: bold;
            }
            @keyframes raylib-spin {
                to { transform: rotate(360deg); }
            }
        `;
        document.head.appendChild(style);
        
        this.container.appendChild(this.loader);
    }
    
    /**
     * Load the game
     * @param {string} gamePath - Path to the WebAssembly game HTML file
     */
    loadGame(gamePath) {
        // Show loader
        if (this.loader) {
            this.loader.style.display = 'flex';
        }
        
        // Load the game in the iframe
        this.iframe.src = gamePath;
        
        // Set up load handling
        this.iframe.onload = () => {
            console.log('WebAssembly game iframe loaded');
            
            // Hide loader after a short delay (gives WebAssembly time to initialize)
            setTimeout(() => {
                if (this.loader) {
                    this.loader.style.display = 'none';
                }
            }, 2000);
            
            // Send initial credits to the game
            this.sendMessage({
                type: 'PORTAL_EVENT',
                action: 'UPDATE_CREDITS',
                amount: this.credits
            });
        };
    }
    
    /**
     * Handle messages from the game
     * @param {MessageEvent} event - Message event
     */
    handleMessage(event) {
        // Only handle messages from our iframe
        if (event.source !== this.iframe.contentWindow) return;
        
        const data = event.data;
        
        if (!data || !data.type) return;
        
        if (data.type === 'GAME_EVENT') {
            console.log('Received game event:', data);
            
            switch (data.action) {
                case 'GAME_READY':
                    console.log('Game is ready to receive messages');
                    this.gameLoaded = true;
                    
                    // Initialize the game with current credits
                    this.sendMessage({
                        type: 'PORTAL_EVENT',
                        action: 'UPDATE_CREDITS',
                        amount: this.credits
                    });
                    break;
                    
                case 'WIN':
                    const winAmount = parseInt(data.amount) || 0;
                    if (winAmount > 0) {
                        this.credits += winAmount;
                        
                        // Update portal credits
                        if (this.portalInterface && this.portalInterface.updateCredits) {
                            this.portalInterface.updateCredits(winAmount, 'raylib slots win');
                        }
                        
                        // Show message
                        if (this.portalInterface && this.portalInterface.showMessage) {
                            this.portalInterface.showMessage(`Won ${winAmount} credits!`, 'success');
                        }
                    }
                    break;
                    
                case 'BET':
                    const betAmount = parseInt(data.amount) || 0;
                    if (betAmount > 0) {
                        this.credits -= betAmount;
                        
                        // Update portal credits
                        if (this.portalInterface && this.portalInterface.updateCredits) {
                            this.portalInterface.updateCredits(-betAmount, 'raylib slots bet');
                        }
                    }
                    break;
                    
                case 'EXIT':
                    // User wants to exit the game
                    if (this.portalInterface && this.portalInterface.closeGame) {
                        this.portalInterface.closeGame();
                    }
                    break;
            }
        }
    }
    
    /**
     * Send a message to the game
     * @param {Object} message - Message to send
     */
    sendMessage(message) {
        if (!this.iframe.contentWindow) return;
        
        this.iframe.contentWindow.postMessage(message, '*');
    }
    
    /**
     * Update credits in the game
     * @param {number} newAmount - New credits amount
     */
    updateCredits(newAmount) {
        this.credits = newAmount;
        
        this.sendMessage({
            type: 'PORTAL_EVENT',
            action: 'UPDATE_CREDITS',
            amount: newAmount
        });
    }
    
    /**
     * Clean up the bridge
     */
    cleanup() {
        window.removeEventListener('message', this.messageHandlerBound);
        
        if (this.container) {
            if (this.loader) {
                this.container.removeChild(this.loader);
            }
            if (this.iframe) {
                this.container.removeChild(this.iframe);
            }
        }
    }
}

// Export the bridge if module.exports is available
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { RaylibBridge };
}