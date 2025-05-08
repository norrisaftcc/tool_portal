/**
 * Game Container Component
 * 
 * This component provides a standardized container for integrating games
 * into the PULSE Play portal. Instead of using iframes, games can be 
 * integrated directly as components for better performance and integration.
 * 
 * Usage:
 * 1. Import your game's initialization function
 * 2. Wrap it in the GameContainer component
 * 3. Pass necessary props like onReturn for navigation
 */

// Game Container Component
class GameContainer extends React.Component {
    constructor(props) {
        super(props);
        this.gameRef = React.createRef();
        this.state = {
            gameLoaded: false,
            error: null
        };
    }

    componentDidMount() {
        // Initialize the game when component mounts
        try {
            if (this.props.gameInitFunction) {
                this.props.gameInitFunction(this.gameRef.current);
                this.setState({ gameLoaded: true });
            }
        } catch (error) {
            console.error("Error initializing game:", error);
            this.setState({ error: "Failed to load game" });
        }
    }

    componentWillUnmount() {
        // Cleanup function if needed
        if (this.props.gameCleanupFunction) {
            try {
                this.props.gameCleanupFunction();
            } catch (error) {
                console.error("Error during game cleanup:", error);
            }
        }
    }

    render() {
        const { title, onReturn } = this.props;
        const { gameLoaded, error } = this.state;

        return (
            <div className="game-component-container">
                {/* Game Header */}
                <div className="game-header bg-gradient-to-r from-indigo-900 to-purple-900 px-4 py-3 flex justify-between items-center shadow-lg">
                    <button 
                        onClick={onReturn}
                        className="flex items-center text-white hover:text-purple-300 transition"
                    >
                        <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className="mr-2">
                            <path d="m12 19-7-7 7-7"/>
                            <path d="M19 12H5"/>
                        </svg>
                        <span>Back to Portal</span>
                    </button>
                    <div className="font-bold text-lg">{title}</div>
                    <div className="w-24"></div> {/* Spacer for balance */}
                </div>
                
                {/* Game Content */}
                <div className="game-content-area flex-1 overflow-hidden">
                    {error && (
                        <div className="error-message bg-red-800 text-white p-4 text-center">
                            {error}
                        </div>
                    )}
                    
                    {!gameLoaded && !error && (
                        <div className="loading-indicator flex items-center justify-center h-full">
                            <div className="text-center">
                                <div className="spinner mb-4 w-12 h-12 border-4 border-blue-500 border-t-transparent rounded-full animate-spin mx-auto"></div>
                                <p className="text-white">Loading game...</p>
                            </div>
                        </div>
                    )}
                    
                    <div 
                        ref={this.gameRef} 
                        className="game-mount-point w-full h-full"
                        style={{ display: gameLoaded && !error ? 'block' : 'none' }}
                    ></div>
                </div>
            </div>
        );
    }
}

// Example usage:
/*
function initBlackjack(containerElement) {
    // Initialize blackjack game in the provided container
    // This would be imported from the actual game file
}

function cleanupBlackjack() {
    // Cleanup resources when game is closed
}

// Then in your main app:
<GameContainer 
    title="Blackjack Blitz" 
    gameInitFunction={initBlackjack} 
    gameCleanupFunction={cleanupBlackjack}
    onReturn={() => setActiveGame(null)}
/>
*/