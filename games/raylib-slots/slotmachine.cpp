#include "raylib.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <random>
#include <algorithm>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_COLUMNS = 3;
const int GRID_ROWS = 3;
const int CELL_SIZE = 100;
const int SPIN_SPEED = 20;
const int offset = 3; // Thickness of the outline

// Global game state that must persist between frames
struct GameState {
    vector<vector<int>> grid;
    vector<Texture2D> fruitTextures;
    Texture2D slotOverlay;
    Texture2D startRelaxedButton;
    Texture2D startPressedButton;
    Texture2D exitRelaxedButton;
    Texture2D exitPressedButton;
    Texture2D resumeRelaxedButton;
    Texture2D resumePressedButton;
    Texture2D menuRelaxedButton;
    Texture2D menuPressedButton;
    Texture2D soundOnIcon;
    Texture2D soundOffIcon;
    Texture2D happyFace;
    Texture2D smileyFace;
    Texture2D blinkingFace;
    
    Sound spinSound;
    Sound winSound;
    Sound buttonClickSound;
    Music menuMusic;

    bool soundMuted;
    bool startGame;
    bool spinning;
    bool quitToMainMenu;
    bool paused;
    bool hasPlayed;
    bool inBonusGame;
    
    int playerBalance;
    string matchMessage;
    float faceTimer;
    int faceIndex;

    GameState() {
        soundMuted = false;
        startGame = false;
        spinning = false;
        quitToMainMenu = false;
        paused = false;
        hasPlayed = false;
        inBonusGame = false;
        playerBalance = 100;
        matchMessage = "";
        faceTimer = 0;
        faceIndex = 0;
    }
};

// Global variables
GameState gameState;

// Function forward declarations
void DrawGrid(const vector<vector<int>>& grid, const vector<Texture2D>& fruitTextures);
bool IsButtonClicked(Rectangle button, Vector2 mousePoint);
void DrawButton(Rectangle button, Vector2 mousePoint, Texture2D relaxedTexture, Texture2D pressedTexture);
void DrawPayoutGrid(const vector<Texture2D>& fruitTextures);
void DrawGameTitle();
void CheckMiddleRowMatch(const vector<vector<int>>& grid, string& matchMessage, int& playerBalance);
vector<vector<int>> GenerateRandomGrid(int textureCount);

// Generate a 2D vector filled with random texture indices
vector<vector<int>> GenerateRandomGrid(int textureCount) {  
    vector<vector<int>> grid(GRID_ROWS, vector<int>(GRID_COLUMNS));  
    random_device rd;  
    mt19937 rng(rd());  

    for (int col = 0; col < GRID_COLUMNS; col++) {  
        vector<int> availableFruits;  
        for (int i = 0; i < textureCount; i++) {  
            availableFruits.push_back(i); // Fill column with all unique fruits  
        }  

        shuffle(availableFruits.begin(), availableFruits.end(), rng); // Shuffle before assigning  

        for (int row = 0; row < GRID_ROWS; row++) {  
            grid[row][col] = availableFruits[row]; // Assign unique fruit for each row  
        }  
    } 

    // Add a controlled chance (~15%) to force a match in the middle row  
    if (rand() % 7 == 0) { // 1 in 7 chance of forcing a match  
        int matchFruit = rand() % textureCount;  
        grid[GRID_ROWS / 2][0] = matchFruit;  
        grid[GRID_ROWS / 2][1] = matchFruit;  
        grid[GRID_ROWS / 2][2] = matchFruit;  
    }  

    return grid;  
}

// Draw the grid with the current textures
void DrawGrid(const vector<vector<int>>& grid, const vector<Texture2D>& fruitTextures) {
    int columnSpacing = 40; // Space between columns
    float fruitWidth = 100;
    float fruitHeight = 80; // Set custom fruit height

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLUMNS; col++) {
            // Adjust grid position & spacing
            int x = (SCREEN_WIDTH / 2 + 102) - (CELL_SIZE * GRID_COLUMNS / 2) + col * (CELL_SIZE + columnSpacing);
            int y = (SCREEN_HEIGHT / 2 - 40) - (CELL_SIZE * GRID_ROWS / 2) + row * CELL_SIZE;

            // Draw grid background
            DrawRectangle(x, y, CELL_SIZE - 3, CELL_SIZE - 3, WHITE);

            // Define the destination rectangle for scaling fruit textures
            Rectangle destRec = {
                x + (CELL_SIZE - fruitWidth) / 2,  // Center horizontally
                y + (CELL_SIZE - fruitHeight) / 2, // Center vertically
                fruitWidth, 
                fruitHeight
            }; 
            Rectangle sourceRec = {0, 0, (float)fruitTextures[grid[row][col]].width, (float)fruitTextures[grid[row][col]].height};
            Vector2 origin = {0, 0};

            // Draw the fruit texture with scaling
            DrawTexturePro(fruitTextures[grid[row][col]], sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
}

// Check if a button is clicked
bool IsButtonClicked(Rectangle button, Vector2 mousePoint) {
    if (CheckCollisionPointRec(mousePoint, button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(gameState.buttonClickSound);
        return true;
    }
    return false;
}

// Draw button with appropriate texture based on mouse interaction
void DrawButton(Rectangle button, Vector2 mousePoint, Texture2D relaxedTexture, Texture2D pressedTexture) {
    Rectangle destRec = {button.x, button.y, button.width, button.height}; // Stretch to button size
    Rectangle sourceRec = {0, 0, (float)relaxedTexture.width, (float)relaxedTexture.height}; // Use full texture
    Vector2 origin = {0, 0}; // No rotation
    
    if (CheckCollisionPointRec(mousePoint, button) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        DrawTexturePro(pressedTexture, sourceRec, destRec, origin, 0.0f, WHITE);
    } else {
        DrawTexturePro(relaxedTexture, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

void DrawPayoutGrid(const vector<Texture2D>& fruitTextures) {
    int startX = 50;
    int startY = 100;
    int spacingY = 50;
    
    vector<int> payouts = {150, 25, 500, 250, 10, 100, 50, 1000}; // Payouts for each fruit

    for (size_t i = 0; i < fruitTextures.size(); i++) {
        // Draw fruit image
        DrawTexture(fruitTextures[i], startX, startY + (i * spacingY), WHITE);
        
        // Display equal sign and payout amount
        DrawText(("= $" + to_string(payouts[i])).c_str(), startX + 50, startY + (i * spacingY) + 10, 20, YELLOW);
    }
}

void DrawGameTitle() {
    // Draw black border behind text (offset in 8 directions)
    DrawText("Fruity", SCREEN_WIDTH / 2 + 60 - offset, 20, 50, BLACK);
    DrawText("Fruity", SCREEN_WIDTH / 2 + 60 + offset, 20, 50, BLACK);
    DrawText("Fruity", SCREEN_WIDTH / 2 + 60, 20 - offset, 50, BLACK);
    DrawText("Fruity", SCREEN_WIDTH / 2 + 60, 20 + offset, 50, BLACK);

    DrawText("Slot Machine", SCREEN_WIDTH / 2 - offset, 70, 50, BLACK);
    DrawText("Slot Machine", SCREEN_WIDTH / 2 + offset, 70, 50, BLACK);
    DrawText("Slot Machine", SCREEN_WIDTH / 2, 70 - offset, 50, BLACK);
    DrawText("Slot Machine", SCREEN_WIDTH / 2, 70 + offset, 50, BLACK);

    // Now draw original title on top of the black border
    DrawText("Fruity", SCREEN_WIDTH / 2 + 60, 20, 50, ORANGE);
    DrawText("Slot Machine", SCREEN_WIDTH / 2, 70, 50, DARKPURPLE);
}

// Check for winning combinations
void CheckMiddleRowMatch(const vector<vector<int>>& grid, string& matchMessage, int& playerBalance) {
    int rowIndex = GRID_ROWS / 2; // Middle row
    int winningSymbol = grid[rowIndex][0];
    
    if (grid[rowIndex][0] == grid[rowIndex][1] && grid[rowIndex][1] == grid[rowIndex][2]) {
        PlaySound(gameState.winSound);
        matchMessage = "You got \n\ta match!";

        // Reward based on fruit type
        int winAmount = 0;
        if (winningSymbol == 7) winAmount = 1000; // Seven
        else if (winningSymbol == 2) winAmount = 500; // Bar
        else if (winningSymbol == 3) winAmount = 250; // Bell
        else if (winningSymbol == 0) winAmount = 150; // Cherry
        else if (winningSymbol == 5) winAmount = 100; // Orange
        else if (winningSymbol == 6) winAmount = 50;  // Pear
        else if (winningSymbol == 1) winAmount = 25;  // Watermelon
        else if (winningSymbol == 4) winAmount = 10;  // Coconut
        
        playerBalance += winAmount;
        
        // Notify JavaScript about the win
        #ifdef __EMSCRIPTEN__
        EM_ASM({
            // Send message to JavaScript with win amount
            const message = {
                type: 'GAME_EVENT',
                action: 'WIN',
                amount: $0,
                symbol: $1
            };
            
            if (window.parent !== window) {
                window.parent.postMessage(message, '*');
            }
        }, winAmount, winningSymbol);
        #endif
    } else {
        matchMessage = "No match!\n\tAgain?";
    }
}

// Handle a spin action
void SpinReels() {
    if (gameState.spinning || gameState.playerBalance < 5) return;
    
    // Deduct bet amount
    gameState.playerBalance -= 5;
    gameState.hasPlayed = true;
    
    // Notify JavaScript about the bet
    #ifdef __EMSCRIPTEN__
    EM_ASM({
        // Send message to JavaScript with bet amount
        const message = {
            type: 'GAME_EVENT',
            action: 'BET',
            amount: 5
        };
        
        if (window.parent !== window) {
            window.parent.postMessage(message, '*');
        }
    });
    #endif
    
    PlaySound(gameState.spinSound);
    
    // Generate new grid
    gameState.grid = GenerateRandomGrid(gameState.fruitTextures.size());
    
    // Check for matches and update the player's balance
    CheckMiddleRowMatch(gameState.grid, gameState.matchMessage, gameState.playerBalance);
}

// Main game update and render function for use with emscripten_set_main_loop
void GameLoop() {
    Vector2 mousePoint = GetMousePosition();
    
    // Handle game input
    if (!gameState.paused && IsKeyPressed(KEY_E)) {
        SpinReels();
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        gameState.paused = !gameState.paused;
    }
    
    if (IsKeyPressed(KEY_M)) {
        gameState.soundMuted = !gameState.soundMuted;
        SetMasterVolume(gameState.soundMuted ? 0.0f : 0.4f);
    }
    
    // Render game
    BeginDrawing();
    ClearBackground(DARKGREEN);
    
    if (gameState.startGame && !gameState.paused) {
        // Draw game elements
        DrawGrid(gameState.grid, gameState.fruitTextures);
        
        // Draw slot machine overlay
        Rectangle destRec = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        Rectangle sourceRec = {0, 0, (float)gameState.slotOverlay.width, (float)gameState.slotOverlay.height};
        Vector2 origin = {0, 0};
        DrawTexturePro(gameState.slotOverlay, sourceRec, destRec, origin, 0.0f, WHITE);
        
        // Draw game title and instructions
        DrawGameTitle();
        DrawText("Press [E] to spin!", SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT - 230, 20, BLACK);
        DrawText("Press [ESC] for Pause Menu", SCREEN_WIDTH - 750, SCREEN_HEIGHT - 570, 11, LIGHTGRAY);
        
        // Draw player balance
        DrawText(("Balance: $" + to_string(gameState.playerBalance)).c_str(), 50, SCREEN_HEIGHT - 50, 20, YELLOW);
        
        // Draw payout table
        DrawPayoutGrid(gameState.fruitTextures);
        
        // Draw message screen
        Rectangle messageScreen = {SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT - 210, 300, 180};
        DrawRectangleRec(messageScreen, BLACK);
        DrawRectangleLinesEx(messageScreen, 5, DARKGREEN);
        
        // Draw animated face or message
        if (!gameState.hasPlayed) {
            gameState.faceTimer += GetFrameTime();
            
            if (gameState.faceTimer > 0.8f) {
                gameState.faceIndex = (gameState.faceIndex + 1) % 3;
                gameState.faceTimer = 0;
            }
            
            // Choose face based on index
            Texture2D currentFace;
            float scaleFactor;
            
            if (gameState.faceIndex == 0) {
                currentFace = gameState.happyFace;
                scaleFactor = 0.4f;
            } else if (gameState.faceIndex == 1) {
                currentFace = gameState.smileyFace;
                scaleFactor = 0.3f;
            } else {
                currentFace = gameState.blinkingFace;
                scaleFactor = 0.35f;
            }
            
            Rectangle faceSourceRec = {0, 0, (float)currentFace.width, (float)currentFace.height};
            Rectangle faceDestRec = {
                messageScreen.x + messageScreen.width / 2 - (currentFace.width * scaleFactor) / 2 - 40,
                messageScreen.y + messageScreen.height / 2 - (currentFace.height * scaleFactor) / 2 + 5,
                currentFace.width * scaleFactor, 
                currentFace.height * scaleFactor
            };
            
            DrawTexturePro(currentFace, faceSourceRec, faceDestRec, {0, 0}, 0.0f, WHITE);
            DrawText("Good", messageScreen.x + messageScreen.width / 2 + 40, messageScreen.y + messageScreen.height / 2 - 10, 30, WHITE);
            DrawText("Luck!", messageScreen.x + messageScreen.width / 2 + 40, messageScreen.y + messageScreen.height / 2 + 25, 30, WHITE);
        } else {
            DrawText(gameState.matchMessage.c_str(), messageScreen.x + 55, messageScreen.y + 40, 45, WHITE);
        }
    } else if (gameState.paused) {
        // Draw pause screen
        Rectangle resumeButton = {SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 60, 200, 100};
        Rectangle menuButton = {SCREEN_WIDTH / 2 + 20, SCREEN_HEIGHT / 2 - 60, 200, 100};
        
        DrawButton(resumeButton, mousePoint, gameState.resumeRelaxedButton, gameState.resumePressedButton);
        DrawButton(menuButton, mousePoint, gameState.menuRelaxedButton, gameState.menuPressedButton);
        
        if (IsButtonClicked(resumeButton, mousePoint)) {
            gameState.paused = false;
        } else if (IsButtonClicked(menuButton, mousePoint)) {
            gameState.paused = false;
            gameState.startGame = false;
            gameState.quitToMainMenu = true;
        }
    } else {
        // Show main menu
        Rectangle playButton = {SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 50, 150, 80};
        Rectangle exitButton = {SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 150, 150, 80};
        Rectangle muteButton = {SCREEN_WIDTH - 60, 20, 40, 40};
        
        // Draw flashing background
        static float scrollOffset = 0;
        scrollOffset += 0.2f;
        if (scrollOffset >= SCREEN_HEIGHT) scrollOffset = 0;
        
        int fruitSize = 80;
        for (int col = 0; col < SCREEN_WIDTH; col += fruitSize) {
            for (int row = -SCREEN_HEIGHT; row < SCREEN_HEIGHT; row += fruitSize) {
                int fruitIndex = (row / fruitSize + col / fruitSize) % gameState.fruitTextures.size();
                DrawTexture(gameState.fruitTextures[fruitIndex], col, row + scrollOffset, WHITE);
            }
        }
        
        // Draw title
        const int offset = 3;
        DrawText("Fruity", SCREEN_WIDTH / 2 - 140 - offset, SCREEN_HEIGHT / 3 - 30, 50, BLACK);
        DrawText("Fruity", SCREEN_WIDTH / 2 - 140 + offset, SCREEN_HEIGHT / 3 - 30, 50, BLACK);
        DrawText("Fruity", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 3 - 30 - offset, 50, BLACK);
        DrawText("Fruity", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 3 - 30 + offset, 50, BLACK);
        
        DrawText("Slot Machine", SCREEN_WIDTH / 2 - 160 - offset, SCREEN_HEIGHT / 3 + 30, 50, BLACK);
        DrawText("Slot Machine", SCREEN_WIDTH / 2 - 160 + offset, SCREEN_HEIGHT / 3 + 30, 50, BLACK);
        DrawText("Slot Machine", SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 3 + 30 - offset, 50, BLACK);
        DrawText("Slot Machine", SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 3 + 30 + offset, 50, BLACK);
        
        DrawText("Fruity", SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 3 - 30, 50, ORANGE);
        DrawText("Slot Machine", SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 3 + 30, 50, DARKPURPLE);
        
        // Draw buttons
        DrawButton(playButton, mousePoint, gameState.startRelaxedButton, gameState.startPressedButton);
        DrawButton(exitButton, mousePoint, gameState.exitRelaxedButton, gameState.exitPressedButton);
        
        // Draw mute button
        DrawTexturePro(
            gameState.soundMuted ? gameState.soundOffIcon : gameState.soundOnIcon,
            {0, 0, (float)gameState.soundOffIcon.width, (float)gameState.soundOffIcon.height},
            {muteButton.x, muteButton.y, muteButton.width, muteButton.height},
            {0, 0}, 0.0f, WHITE
        );
        
        // Handle button clicks
        if (IsButtonClicked(playButton, mousePoint)) {
            gameState.startGame = true;
            gameState.hasPlayed = false;
            gameState.faceIndex = 0;
            gameState.faceTimer = 0;
            gameState.matchMessage = "";
            gameState.grid = GenerateRandomGrid(gameState.fruitTextures.size());
        } else if (IsButtonClicked(exitButton, mousePoint)) {
            // In a web context, we can't just exit
            #ifdef __EMSCRIPTEN__
            EM_ASM({
                // Send message to JavaScript to go back to portal
                const message = {
                    type: 'GAME_EVENT',
                    action: 'EXIT'
                };
                
                if (window.parent !== window) {
                    window.parent.postMessage(message, '*');
                }
            });
            #else
            // On desktop, we would exit
            CloseWindow();
            #endif
        } else if (IsButtonClicked(muteButton, mousePoint)) {
            gameState.soundMuted = !gameState.soundMuted;
            SetMasterVolume(gameState.soundMuted ? 0.0f : 0.4f);
        }
    }
    
    EndDrawing();
}

// Function to update credits from JavaScript
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void updateCredits(int newBalance) {
        gameState.playerBalance = newBalance;
    }
    
    EMSCRIPTEN_KEEPALIVE
    int getCredits() {
        return gameState.playerBalance;
    }
    
    EMSCRIPTEN_KEEPALIVE
    void forceWin() {
        // Force a win with 7's (highest payout)
        for (int col = 0; col < GRID_COLUMNS; col++) {
            gameState.grid[GRID_ROWS / 2][col] = 7; // Seven
        }
        
        CheckMiddleRowMatch(gameState.grid, gameState.matchMessage, gameState.playerBalance);
    }
}

// Main entry point
int main() {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Slot Machine");
    SetTargetFPS(60);
    
    // Initialize audio
    InitAudioDevice();
    SetMasterVolume(0.4f);
    
    // Load textures
    // Fruit textures
    vector<string> textureFiles = {
        "cherry.png", "watermelon.png", "bar.png", "bell.png",
        "coconut.png", "orange.png", "pear.png", "seven.png"
    };
    
    for (const auto& file : textureFiles) {
        gameState.fruitTextures.push_back(LoadTexture(("images/" + file).c_str()));
    }
    
    // UI textures
    gameState.slotOverlay = LoadTexture("images/pixel_art_slot_machine_-_machine_frame2.png");
    gameState.startRelaxedButton = LoadTexture("images/startRelaxedButton.png");
    gameState.startPressedButton = LoadTexture("images/startPressedButton.png");
    gameState.exitRelaxedButton = LoadTexture("images/exitRelaxedButton.png");
    gameState.exitPressedButton = LoadTexture("images/exitPressedButton.png");
    gameState.resumeRelaxedButton = LoadTexture("images/resumeRelaxedButton.png");
    gameState.resumePressedButton = LoadTexture("images/resumePressedButton.png");
    gameState.menuRelaxedButton = LoadTexture("images/menuRelaxedButton.png");
    gameState.menuPressedButton = LoadTexture("images/menuPressedButton.png");
    gameState.soundOnIcon = LoadTexture("images/soundOn.png");
    gameState.soundOffIcon = LoadTexture("images/soundOff.png");
    gameState.happyFace = LoadTexture("images/happyFace.png");
    gameState.smileyFace = LoadTexture("images/smileyFace.png");
    gameState.blinkingFace = LoadTexture("images/blinkingFace.png");
    
    // Load sounds
    gameState.spinSound = LoadSound("sounds/spin.mp3");
    gameState.winSound = LoadSound("sounds/win.ogg");
    gameState.buttonClickSound = LoadSound("sounds/buttonClick.ogg");
    
    // Initialize game state
    gameState.grid = GenerateRandomGrid(gameState.fruitTextures.size());
    
    // Notify JavaScript that game is ready
    #ifdef __EMSCRIPTEN__
    EM_ASM({
        // Send message to JavaScript that game is ready
        const message = {
            type: 'GAME_EVENT',
            action: 'GAME_READY'
        };
        
        if (window.parent !== window) {
            window.parent.postMessage(message, '*');
        }
        
        // Listen for messages from parent (portal)
        window.addEventListener('message', function(event) {
            // Verify message is from parent
            if (event.source !== window.parent) return;
            
            const data = event.data;
            
            // Handle portal messages
            if (data && data.type === 'PORTAL_EVENT') {
                switch (data.action) {
                    case 'UPDATE_CREDITS':
                        // Update credits from portal
                        Module.ccall('updateCredits', null, ['number'], [data.amount]);
                        break;
                        
                    case 'FORCE_WIN':
                        // Force a win (for testing)
                        Module.ccall('forceWin', null, [], []);
                        break;
                }
            }
        });
    });
    #endif
    
    // Set up the main loop
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(GameLoop, 0, 1);
    #else
    // For non-web platforms, use a standard game loop
    while (!WindowShouldClose()) {
        GameLoop();
    }
    
    // Cleanup (only needed for non-web)
    for (auto& texture : gameState.fruitTextures) {
        UnloadTexture(texture);
    }
    
    UnloadTexture(gameState.slotOverlay);
    UnloadTexture(gameState.startRelaxedButton);
    UnloadTexture(gameState.startPressedButton);
    UnloadTexture(gameState.exitRelaxedButton);
    UnloadTexture(gameState.exitPressedButton);
    UnloadTexture(gameState.resumeRelaxedButton);
    UnloadTexture(gameState.resumePressedButton);
    UnloadTexture(gameState.menuRelaxedButton);
    UnloadTexture(gameState.menuPressedButton);
    UnloadTexture(gameState.soundOnIcon);
    UnloadTexture(gameState.soundOffIcon);
    UnloadTexture(gameState.happyFace);
    UnloadTexture(gameState.smileyFace);
    UnloadTexture(gameState.blinkingFace);
    
    UnloadSound(gameState.spinSound);
    UnloadSound(gameState.winSound);
    UnloadSound(gameState.buttonClickSound);
    
    CloseAudioDevice();
    CloseWindow();
    #endif
    
    return 0;
}