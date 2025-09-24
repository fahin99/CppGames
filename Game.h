#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "Player.h"
#include "Dungeon.h"
#include "Camera.h"
#include "Enemy.h"
#include "GameState.h"
#include "UserManager.h"
#include "TransitionManager.h"
#include "PowerUp.h"

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    sf::Font font;
    
    std::unique_ptr<Player> player;
    std::unique_ptr<Dungeon> dungeon;
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    std::unique_ptr<UserManager> userManager;
    std::unique_ptr<TransitionManager> transitionManager;
    
    GameState currentState;
    int score;
    int currentLevel;
    int treasuresCollected;
    int enemiesKilled;
    int initialEnemyCount;
    bool isRunning;
    
    // UI input handling
    std::string inputText;
    std::string username;
    std::string password;
    std::string playerName;
    bool isTyping;
    
    // Input field states
    enum class InputField {
        NONE,
        USERNAME,
        PASSWORD,
        PLAYER_NAME
    } currentInputField;
    
public:
    Game();
    ~Game();
    
    void run();
    void handleEvents();
    void handleKeyPress(sf::Keyboard::Key key);
    void handleMouseClick(int x, int y);
    void update(float deltaTime);
    void render();
    void initializeGame();
    void resetGame();
    void generateLevel();
    void updateEnemies(float deltaTime);
    void updatePowerUps(float deltaTime);
    void generatePowerUps();
    void checkVictoryConditions();
    int getTotalTreasures();
    int getInitialEnemyCount();
    void nextLevel();
    void renderUI();
    
    // New UI methods
    void renderWelcomeScreen();
    void renderLoginScreen();
    void renderRegisterScreen();
    void renderPlayerNameScreen();
    void renderPauseScreen();
    void renderVictoryScreen();
    void renderGameOverScreen();
    void handleTextInput(sf::Uint32 unicode);
    void drawInputField(sf::RenderWindow& window, const std::string& label, const std::string& text, 
                       float x, float y, bool isActive, bool isPassword = false);
    void drawButton(sf::RenderWindow& window, const std::string& text, float x, float y, 
                   float width, float height, sf::Color color = sf::Color::Blue);
    void drawEnhancedInputField(sf::RenderWindow& window, const std::string& label, const std::string& text, 
                               float x, float y, bool isActive, bool isPassword = false);
    void drawEnhancedButton(sf::RenderWindow& window, const std::string& text, float x, float y, 
                           float width, float height, sf::Color baseColor, sf::Color highlightColor);
    void transitionToState(GameState newState);
    
    // Collision detection
    bool checkWallCollision(sf::Vector2f position, sf::Vector2f size);
    
    // Text rendering
    void drawSimpleText(sf::RenderWindow& window, const std::string& text, float x, float y);
    
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 800;
};