#include "Game.h"
#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <cstring>

const int Game::WINDOW_WIDTH;
const int Game::WINDOW_HEIGHT;

Game::Game() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dungeon Crawler", sf::Style::Close)
    , currentState(GameState::WELCOME)
    , score(0)
    , currentLevel(1)
    , treasuresCollected(0)
    , enemiesKilled(0)
    , initialEnemyCount(0)
    , isRunning(true) {
    
    window.setFramerateLimit(60);
    
    // Try to load font (optional - will use default if fails)
    if (!font.loadFromFile("arial.ttf")) {
        // Use system default if available, or continue without font
        std::cout << "Warning: Could not load font file\n";
    }
    
    initializeGame();
}

Game::~Game() {
    // Smart pointers will automatically clean up
}

void Game::run() {
    std::cout << "Starting game loop..." << std::endl;
    
    while (window.isOpen() && isRunning) {
        float deltaTime = clock.restart().asSeconds();
        
        handleEvents();
        
        // Update transition manager
        if (transitionManager) {
            transitionManager->update(deltaTime);
            // Only use transition manager state if we're in a transition
            if (transitionManager->isTransitioning()) {
                currentState = transitionManager->getCurrentState();
            }
        }
        
        if (currentState == GameState::PLAYING) {
            update(deltaTime);
        }
        
        render();
        
        // Add a small delay to prevent excessive CPU usage
        sf::sleep(sf::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "Game loop ended." << std::endl;
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                isRunning = false;
                break;
                
            case sf::Event::TextEntered:
                if (isTyping) {
                    handleTextInput(event.text.unicode);
                }
                break;
                
            case sf::Event::KeyPressed:
                handleKeyPress(event.key.code);
                break;
                
            case sf::Event::MouseButtonPressed:
                handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                break;
                
            default:
                break;
        }
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
    switch (currentState) {
        case GameState::WELCOME:
            if (key == sf::Keyboard::Escape) {
                window.close();
                isRunning = false;
            }
            break;
            
        case GameState::LOGIN:
        case GameState::REGISTER:
        case GameState::PLAYER_NAME:
            if (key == sf::Keyboard::Tab) {
                // Switch input fields
                if (currentState == GameState::LOGIN || currentState == GameState::REGISTER) {
                    currentInputField = (currentInputField == InputField::USERNAME) ? 
                                       InputField::PASSWORD : InputField::USERNAME;
                }
            } else if (key == sf::Keyboard::Enter) {
                if (currentState == GameState::LOGIN) {
                    if (userManager && userManager->loginUser(username, password)) {
                        // Existing user - go directly to game
                        playerName = username;
                        transitionToState(GameState::PLAYING);
                        resetGame();
                    }
                } else if (currentState == GameState::REGISTER) {
                    if (userManager && userManager->registerUser(username, password)) {
                        // New user - ask for display name
                        transitionToState(GameState::PLAYER_NAME);
                    }
                } else if (currentState == GameState::PLAYER_NAME) {
                    if (!playerName.empty()) {
                        transitionToState(GameState::PLAYING);
                        resetGame();
                    }
                }
            } else if (key == sf::Keyboard::Escape) {
                if (currentState == GameState::LOGIN || currentState == GameState::REGISTER) {
                    transitionToState(GameState::WELCOME);
                } else if (currentState == GameState::PLAYER_NAME) {
                    transitionToState(GameState::WELCOME);
                }
            }
            break;
            
        case GameState::MENU:
            if (key == sf::Keyboard::Space) {
                currentState = GameState::PLAYING;
                resetGame();
            }
            break;
            
        case GameState::PLAYING:
            if (key == sf::Keyboard::Escape) {
                currentState = GameState::PAUSED;
            }
            break;
            
        case GameState::PAUSED:
            if (key == sf::Keyboard::Escape || key == sf::Keyboard::Space) {
                currentState = GameState::PLAYING; // Resume
            } else if (key == sf::Keyboard::R) {
                currentState = GameState::PLAYING;
                resetGame(); // Restart
            } else if (key == sf::Keyboard::Q) {
                currentState = GameState::WELCOME; // Quit to menu
                currentLevel = 1;
                treasuresCollected = 0;
                enemiesKilled = 0;
                score = 0;
            }
            break;
            
        case GameState::GAME_OVER:
            if (key == sf::Keyboard::R) {
                currentState = GameState::PLAYING;
                resetGame();
            } else if (key == sf::Keyboard::Escape) {
                currentState = GameState::WELCOME;
            }
            break;
            
        case GameState::VICTORY:
            if (key == sf::Keyboard::Space || key == sf::Keyboard::Enter) {
                nextLevel();
            } else if (key == sf::Keyboard::Escape) {
                currentState = GameState::WELCOME;
                // Reset to level 1
                currentLevel = 1;
                treasuresCollected = 0;
                enemiesKilled = 0;
            }
            break;
    }
}

void Game::handleMouseClick(int x, int y) {
    std::cout << "Mouse clicked at: " << x << ", " << y << " in state " << (int)currentState << std::endl;
    
    switch (currentState) {
        case GameState::WELCOME:
            std::cout << "Checking welcome screen buttons..." << std::endl;
            // Check button clicks - updated coordinates to match enhanced layout
            if (x >= WINDOW_WIDTH/2 - 120 && x <= WINDOW_WIDTH/2 + 120) {
                if (y >= 320 && y <= 380) { // Enhanced Login button
                    std::cout << "Login button clicked!" << std::endl;
                    isTyping = true;
                    currentInputField = InputField::USERNAME;
                    username.clear();
                    password.clear();
                    currentState = GameState::LOGIN; // Direct state change for now
                } else if (y >= 410 && y <= 470) { // Enhanced Register button
                    std::cout << "Register button clicked!" << std::endl;
                    isTyping = true;
                    currentInputField = InputField::USERNAME;
                    username.clear();
                    password.clear();
                    currentState = GameState::REGISTER; // Direct state change for now
                }
            }
            break;
            
        case GameState::LOGIN:
        case GameState::REGISTER:
            std::cout << "Checking login/register screen clicks..." << std::endl;
            // Check input field clicks (updated coordinates to match enhanced layout)
            if (x >= WINDOW_WIDTH/2 - 200 && x <= WINDOW_WIDTH/2 + 200) {
                if (y >= 240 && y <= 280) { // Enhanced Username field
                    std::cout << "Username field clicked!" << std::endl;
                    currentInputField = InputField::USERNAME;
                    isTyping = true;
                } else if (y >= 320 && y <= 360) { // Enhanced Password field
                    std::cout << "Password field clicked!" << std::endl;
                    currentInputField = InputField::PASSWORD;
                    isTyping = true;
                }
            }
            
            // Check button clicks (updated coordinates for enhanced layout)
            if (x >= WINDOW_WIDTH/2 - 100 && x <= WINDOW_WIDTH/2 + 100) {
                if (y >= 400 && y <= 450) { // Enhanced Action button
                    std::cout << "Action button clicked!" << std::endl;
                    if (currentState == GameState::LOGIN) {
                        if (userManager && userManager->loginUser(username, password)) {
                            std::cout << "Login successful!" << std::endl;
                            // Existing user - use username as player name and go directly to game
                            playerName = username;
                            isTyping = false;
                            std::cout << "Welcome back, " << playerName << "!" << std::endl;
                            currentState = GameState::PLAYING;
                            resetGame();
                        } else {
                            std::cout << "Login failed!" << std::endl;
                        }
                    } else if (currentState == GameState::REGISTER) {
                        if (userManager && userManager->registerUser(username, password)) {
                            std::cout << "Registration successful!" << std::endl;
                            // New user - ask for display name
                            isTyping = true;
                            currentInputField = InputField::PLAYER_NAME;
                            playerName.clear();
                            currentState = GameState::PLAYER_NAME;
                        } else {
                            std::cout << "Registration failed!" << std::endl;
                        }
                    }
                } else if (y >= 470 && y <= 520) { // Enhanced Back button
                    std::cout << "Back button clicked!" << std::endl;
                    isTyping = false;
                    currentState = GameState::WELCOME;
                }
            }
            break;
            
        case GameState::PLAYER_NAME:
            std::cout << "Checking player name screen clicks..." << std::endl;
            // Check input field click (updated for enhanced layout)
            if (x >= WINDOW_WIDTH/2 - 200 && x <= WINDOW_WIDTH/2 + 200 &&
                y >= 280 && y <= 320) {
                std::cout << "Player name field clicked!" << std::endl;
                currentInputField = InputField::PLAYER_NAME;
                isTyping = true;
            }
            
            // Check start button (coordinates must match the render function)
            if (x >= WINDOW_WIDTH/2 - 100 && x <= WINDOW_WIDTH/2 + 100 &&
                y >= 350 && y <= 400) { // Updated to match actual button position
                std::cout << "START GAME button clicked!" << std::endl;
                if (!playerName.empty()) {
                    std::cout << "Starting game with player: " << playerName << std::endl;
                    isTyping = false;
                    currentState = GameState::PLAYING; // Direct state change instead of transition
                    // Force initialize/reset game for playing
                    std::cout << "Initializing game world..." << std::endl;
                    initializeGame();
                    resetGame();
                    std::cout << "Game world initialized." << std::endl;
                } else {
                    std::cout << "Player name is empty!" << std::endl;
                }
            }
            break;
            
        case GameState::PAUSED:
            std::cout << "Checking pause screen clicks..." << std::endl;
            // Check button clicks
            if (x >= WINDOW_WIDTH/2 - 100 && x <= WINDOW_WIDTH/2 + 100) {
                float buttonY = WINDOW_HEIGHT/2 - 80;
                if (y >= buttonY && y <= buttonY + 50) { // Resume button
                    std::cout << "Resume button clicked!" << std::endl;
                    currentState = GameState::PLAYING;
                } else if (y >= buttonY + 70 && y <= buttonY + 120) { // Restart button
                    std::cout << "Restart button clicked!" << std::endl;
                    currentState = GameState::PLAYING;
                    resetGame();
                } else if (y >= buttonY + 140 && y <= buttonY + 190) { // Quit to Menu button
                    std::cout << "Quit to Menu button clicked!" << std::endl;
                    currentState = GameState::WELCOME;
                    // Reset game state
                    currentLevel = 1;
                    treasuresCollected = 0;
                    enemiesKilled = 0;
                    score = 0;
                }
            }
            break;
            
        case GameState::VICTORY:
            std::cout << "Checking victory screen clicks..." << std::endl;
            // Check button clicks
            if (x >= WINDOW_WIDTH/2 - 100 && x <= WINDOW_WIDTH/2 + 100) {
                if (y >= 550 && y <= 600) { // Continue button
                    std::cout << "Continue button clicked!" << std::endl;
                    nextLevel();
                } else if (y >= 620 && y <= 670) { // Main Menu button
                    std::cout << "Main Menu button clicked!" << std::endl;
                    currentState = GameState::WELCOME;
                    // Reset to level 1
                    currentLevel = 1;
                    treasuresCollected = 0;
                    enemiesKilled = 0;
                }
            }
            break;
            
        case GameState::GAME_OVER:
            std::cout << "Checking game over screen clicks..." << std::endl;
            // Check button clicks
            if (y >= 550 && y <= 595) {
                if (x >= WINDOW_WIDTH/2 - 120 && x <= WINDOW_WIDTH/2 - 20) { // Restart button
                    std::cout << "Restart button clicked!" << std::endl;
                    currentState = GameState::PLAYING;
                    resetGame();
                } else if (x >= WINDOW_WIDTH/2 + 20 && x <= WINDOW_WIDTH/2 + 120) { // Menu button
                    std::cout << "Menu button clicked!" << std::endl;
                    currentState = GameState::WELCOME;
                    // Reset to level 1
                    currentLevel = 1;
                    treasuresCollected = 0;
                    enemiesKilled = 0;
                }
            }
            break;
    }
}

void Game::update(float deltaTime) {
    if (player && player->isAlive()) {
        player->update(deltaTime);
        
        // Proper collision detection with boundaries
        sf::Vector2f newPos = player->getPosition();
        sf::FloatRect playerBounds = player->getBounds();
        
        // Store previous position before movement
        static sf::Vector2f previousPosition = newPos;
        static bool firstUpdate = true;
        
        if (!firstUpdate) {
            // Check collision with walls using player bounds
            if (checkWallCollision(newPos, sf::Vector2f(playerBounds.width, playerBounds.height))) {
                // Revert to previous position
                player->setPosition(previousPosition);
            } else {
                // Update previous position if no collision
                previousPosition = newPos;
            }
        } else {
            previousPosition = newPos;
            firstUpdate = false;
        }
        
        // Check treasure collection
        TileType currentTile = dungeon->getTileType(newPos.x, newPos.y);
        if (currentTile == TileType::TREASURE) {
            // Collect treasure
            score += 500;
            treasuresCollected++;
            player->gainExperience(50);
            
            std::cout << "Treasure collected! Total: " << treasuresCollected << std::endl;
            
            // Remove treasure (convert to floor)
            dungeon->setTileTypeAt(newPos.x, newPos.y, TileType::FLOOR);
            
            if (camera) {
                camera->shake(3.0f, 0.3f);
            }
        }
        
        // Update camera to follow player
        if (camera) {
            camera->update(player->getPosition(), deltaTime);
        }
        
        // Update enemies
        updateEnemies(deltaTime);
        
        // Update power-ups
        updatePowerUps(deltaTime);
        
        // Check victory conditions for level progression
        checkVictoryConditions();
        
        // Check if player is dead
        if (!player->isAlive()) {
            currentState = GameState::GAME_OVER;
        }
    }
    
    // Update transition manager
    if (transitionManager) {
        transitionManager->update(deltaTime);
        if (transitionManager->shouldChangeState()) {
            currentState = transitionManager->getTargetState();
        }
    }
}

void Game::render() {
    window.clear(sf::Color(176, 224, 230)); // Light teal background
    
    switch (currentState) {
        case GameState::WELCOME:
            renderWelcomeScreen();
            break;
            
        case GameState::LOGIN:
            renderLoginScreen();
            break;
            
        case GameState::REGISTER:
            renderRegisterScreen();
            break;
            
        case GameState::PLAYER_NAME:
            renderPlayerNameScreen();
            break;
            
        case GameState::MENU: {
            // Draw simple colored rectangles instead of text for now
            sf::RectangleShape titleBg(sf::Vector2f(400, 60));
            titleBg.setFillColor(sf::Color::Blue);
            titleBg.setPosition(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 100);
            
            sf::RectangleShape instructionBg(sf::Vector2f(240, 30));
            instructionBg.setFillColor(sf::Color::Green);
            instructionBg.setPosition(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2);
            
            window.draw(titleBg);
            window.draw(instructionBg);
            break;
        }
        
        case GameState::PLAYING: {
            // Set camera view
            if (camera) {
                camera->setView(window);
            }
            
            // Render game world
            if (dungeon) {
                dungeon->render(window, camera ? camera->getView() : window.getDefaultView());
            }
            
            if (player) {
                player->render(window);
            }
            
            // Render enemies
            for (auto& enemy : enemies) {
                if (enemy && enemy->isAlive()) {
                    enemy->render(window);
                }
            }
            
            // Render power-ups
            for (auto& powerUp : powerUps) {
                if (powerUp && !powerUp->isCollected()) {
                    powerUp->render(window);
                }
            }
            
            // Reset to default view for UI
            window.setView(window.getDefaultView());
            renderUI();
            break;
        }
        
        case GameState::PAUSED:
            renderPauseScreen();
            break;
        
        case GameState::GAME_OVER:
            renderGameOverScreen();
            break;
        
        case GameState::VICTORY:
            renderVictoryScreen();
            break;
        
        default:
            break;
    }
    
    // Render transition effects on top
    if (transitionManager) {
        transitionManager->render(window);
    }
    
    window.display();
}

void Game::initializeGame() {
    std::cout << "Initializing game..." << std::endl;
    // Initialize game systems
    camera = std::make_unique<Camera>(WINDOW_WIDTH, WINDOW_HEIGHT);
    dungeon = std::make_unique<Dungeon>(60, 45); // Compact but complex dungeon
    userManager = std::make_unique<UserManager>();
    transitionManager = std::make_unique<TransitionManager>(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Initialize input handling
    isTyping = false;
    currentInputField = InputField::NONE;
    
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::resetGame() {
    std::cout << "Resetting game..." << std::endl;
    
    // Reset level progress (only when starting new game, not between levels)
    if (currentState != GameState::VICTORY) {
        currentLevel = 1;
        treasuresCollected = 0;
        enemiesKilled = 0;
        score = 0;
    }
    
    // Generate new level
    generateLevel();
    
    // Create player at spawn point
    sf::Vector2f spawnPoint = dungeon->getPlayerSpawn();
    std::cout << "Player spawn: " << spawnPoint.x << ", " << spawnPoint.y << std::endl;
    player = std::make_unique<Player>(spawnPoint.x, spawnPoint.y);
    
    // Clear and regenerate enemies
    enemies.clear();
    int enemyCount = 6 + (currentLevel - 1) * 2; // Scale with level: 6, 8, 10, 12...
    enemyCount = std::min(enemyCount, 12); // Cap at 12 enemies
    
    std::vector<sf::Vector2f> enemySpawns = dungeon->getEnemySpawns(enemyCount);
    initialEnemyCount = enemySpawns.size(); // Track actual enemy count
    
    std::cout << "Generated " << initialEnemyCount << " enemies for level " << currentLevel << std::endl;
    for (const auto& spawn : enemySpawns) {
        // Randomly choose enemy type
        EnemyType type = static_cast<EnemyType>(rand() % 3);
        enemies.push_back(std::make_unique<Enemy>(type, spawn.x, spawn.y));
    }
    
    // Generate power-ups
    generatePowerUps();
}

void Game::generateLevel() {
    if (dungeon) {
        dungeon->generate();
    }
}

void Game::checkVictoryConditions() {
    // Clear and consistent victory conditions:
    // 1. Collect at least 2 treasures (minimum progress requirement)
    // 2. AND defeat at least 50% of enemies OR collect ALL treasures
    
    int totalTreasures = getTotalTreasures();
    int totalEnemies = initialEnemyCount;
    
    bool minimumTreasures = (treasuresCollected >= 2);
    bool allTreasuresCollected = (treasuresCollected >= totalTreasures);
    bool majorityEnemiesKilled = (enemiesKilled >= totalEnemies / 2);
    
    // Victory condition: minimum treasures AND (majority enemies killed OR all treasures)
    bool levelComplete = minimumTreasures && (majorityEnemiesKilled || allTreasuresCollected);
    
    if (levelComplete) {
        std::cout << "=== LEVEL " << currentLevel << " COMPLETED! ===" << std::endl;
        std::cout << "Treasures collected: " << treasuresCollected << "/" << totalTreasures << std::endl;
        std::cout << "Enemies defeated: " << enemiesKilled << "/" << totalEnemies << std::endl;
        std::cout << "Victory condition met!" << std::endl;
        
        currentLevel++;
        currentState = GameState::VICTORY;
    }
}

int Game::getTotalTreasures() {
    // Count treasures in current dungeon
    int totalTreasures = 0;
    if (dungeon) {
        // Estimate based on room count (roughly 30% of rooms have treasure, minimum 3)
        totalTreasures = std::max(3, (int)(dungeon->getRoomCount() * 0.3f));
    }
    return totalTreasures;
}

int Game::getInitialEnemyCount() {
    return initialEnemyCount;
}

void Game::nextLevel() {
    std::cout << "Advancing to level " << currentLevel << "!" << std::endl;
    
    // Reset level stats
    treasuresCollected = 0;
    enemiesKilled = 0;
    
    // Generate new level with increased difficulty
    generateLevel();
    
    // Reset player position
    if (dungeon && player) {
        sf::Vector2f spawnPoint = dungeon->getPlayerSpawn();
        player->setPosition(spawnPoint);
    }
    
    // Regenerate enemies with increased difficulty
    enemies.clear();
    powerUps.clear(); // Clear old power-ups
    if (dungeon) {
        int enemyCount = 6 + (currentLevel - 1) * 2; // Scale with level: 6, 8, 10, 12...
        enemyCount = std::min(enemyCount, 14); // Cap at 14 enemies
        
        std::vector<sf::Vector2f> enemySpawns = dungeon->getEnemySpawns(enemyCount);
        initialEnemyCount = enemySpawns.size(); // Track actual enemy count
        
        std::cout << "Generated " << initialEnemyCount << " enemies for level " << currentLevel << std::endl;
        
        for (const auto& spawn : enemySpawns) {
            EnemyType type = static_cast<EnemyType>(rand() % 3);
            enemies.push_back(std::make_unique<Enemy>(type, spawn.x, spawn.y));
        }
    }
    
    // Resume playing
    currentState = GameState::PLAYING;
}

void Game::updateEnemies(float deltaTime) {
    for (auto it = enemies.begin(); it != enemies.end();) {
        if ((*it)->isDead()) {
            // Award experience for killing enemy
            if (player) {
                player->gainExperience((*it)->getExperienceReward());
                score += 100;
                enemiesKilled++;
                std::cout << "Enemy killed! Total: " << enemiesKilled << std::endl;
            }
            it = enemies.erase(it);
        } else {
            if (player) {
                (*it)->update(deltaTime, *player);
                
                // Check for combat
                float distance = std::sqrt(std::pow((*it)->getPosition().x - player->getPosition().x, 2) + 
                                         std::pow((*it)->getPosition().y - player->getPosition().y, 2));
                
                // Enemy attacks player
                if (distance <= 40.0f) { // Attack range
                    (*it)->attack(*player);
                }
                
                // Player attacks enemy when attacking and in range
                if (player->getIsAttacking() && distance <= 60.0f) {
                    int damage = player->getEffectiveAttack();
                    (*it)->takeDamage(damage);
                    
                    std::cout << "Player attacked enemy for " << damage << " damage! Enemy health: " << (*it)->getStats().health << "/" << (*it)->getStats().maxHealth << std::endl;
                    
                    // Camera shake on hit
                    if (camera) {
                        camera->shake(5.0f, 0.2f);
                    }
                }
            }
            ++it;
        }
    }
}

void Game::generatePowerUps() {
    powerUps.clear();
    
    if (!dungeon) return;
    
    // Generate 3-5 power-ups per level
    int powerUpCount = 3 + (rand() % 3);
    
    for (int i = 0; i < powerUpCount; i++) {
        // Get random empty position
        sf::Vector2f pos;
        int attempts = 0;
        do {
            pos.x = 50 + rand() % (dungeon->getWidth() * 20 - 100);
            pos.y = 50 + rand() % (dungeon->getHeight() * 20 - 100);
            attempts++;
        } while (dungeon->isWall(pos.x, pos.y) && attempts < 50);
        
        if (attempts < 50) {
            // Random power-up type
            PowerUpType type = static_cast<PowerUpType>(rand() % 4);
            powerUps.push_back(std::make_unique<PowerUp>(type, pos.x, pos.y));
        }
    }
    
    std::cout << "Generated " << powerUps.size() << " power-ups for level " << currentLevel << std::endl;
}

void Game::updatePowerUps(float deltaTime) {
    if (!player) return;
    
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        if ((*it)->checkCollision(player->getPosition(), Player::PLAYER_SIZE)) {
            // Player collected power-up
            PowerUpType type = (*it)->getType();
            int value = (*it)->getEffectValue();
            float duration = (*it)->getEffectDuration();
            
            player->applyPowerUp(static_cast<int>(type), value, duration);
            
            // Visual/audio feedback
            std::string powerUpName;
            switch (type) {
                case PowerUpType::HEALTH_POTION: powerUpName = "Health Potion"; break;
                case PowerUpType::DAMAGE_BOOST: powerUpName = "Damage Boost"; break;
                case PowerUpType::SPEED_BOOST: powerUpName = "Speed Boost"; break;
                case PowerUpType::ARMOR_BOOST: powerUpName = "Armor Boost"; break;
            }
            
            std::cout << "Collected " << powerUpName << "!" << std::endl;
            
            it = powerUps.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::renderUI() {
    if (!player) return;
    
    Stats stats = player->getStats();
    
    // Health bar
    sf::RectangleShape healthBarBg(sf::Vector2f(200, 20));
    healthBarBg.setPosition(10, 10);
    healthBarBg.setFillColor(sf::Color::Red);
    
    sf::RectangleShape healthBar(sf::Vector2f(200 * stats.health / stats.maxHealth, 20));
    healthBar.setPosition(10, 10);
    healthBar.setFillColor(sf::Color::Green);
    
    // Level indicator (colored rectangles)
    for (int i = 0; i < stats.level; i++) {
        sf::RectangleShape levelIndicator(sf::Vector2f(15, 15));
        levelIndicator.setPosition(10 + i * 20, 40);
        levelIndicator.setFillColor(sf::Color::Yellow);
        window.draw(levelIndicator);
    }
    
    // Experience bar
    sf::RectangleShape expBarBg(sf::Vector2f(200, 10));
    expBarBg.setPosition(10, 65);
    expBarBg.setFillColor(sf::Color::Blue);
    
    int expNeeded = stats.level * 100;
    sf::RectangleShape expBar(sf::Vector2f(200 * stats.experience / expNeeded, 10));
    expBar.setPosition(10, 65);
    expBar.setFillColor(sf::Color::Cyan);
    
    window.draw(healthBarBg);
    window.draw(healthBar);
    window.draw(expBarBg);
    window.draw(expBar);
    
    // Level progression info (top right)
    sf::RectangleShape infoBg(sf::Vector2f(250, 120));
    infoBg.setPosition(WINDOW_WIDTH - 260, 10);
    infoBg.setFillColor(sf::Color(30, 30, 30, 180));
    infoBg.setOutlineThickness(2);
    infoBg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(infoBg);
    
    // Current level
    std::string levelText = "LEVEL " + std::to_string(currentLevel);
    drawSimpleText(window, levelText, WINDOW_WIDTH - 250, 20);
    
    // Treasures collected
    std::string treasureText = "Treasures: " + std::to_string(treasuresCollected) + "/" + std::to_string(getTotalTreasures());
    drawSimpleText(window, treasureText, WINDOW_WIDTH - 250, 40);
    
    // Enemies defeated
    std::string enemyText = "Enemies: " + std::to_string(enemiesKilled) + "/" + std::to_string(initialEnemyCount);
    drawSimpleText(window, enemyText, WINDOW_WIDTH - 250, 60);
    
    // Victory requirements - more prominent
    sf::RectangleShape objectiveBg(sf::Vector2f(240, 60));
    objectiveBg.setPosition(WINDOW_WIDTH - 250, 80);
    objectiveBg.setFillColor(sf::Color(0, 100, 150, 200));
    objectiveBg.setOutlineThickness(2);
    objectiveBg.setOutlineColor(sf::Color::Cyan);
    window.draw(objectiveBg);
    
    drawSimpleText(window, "OBJECTIVES TO ADVANCE:", WINDOW_WIDTH - 245, 90);
    drawSimpleText(window, "- Collect 2+ treasures", WINDOW_WIDTH - 245, 105);
    drawSimpleText(window, "- Kill 50% of enemies", WINDOW_WIDTH - 245, 120);
    
    // Show current progress toward objectives
    bool treasureObjective = treasuresCollected >= 2;
    bool enemyObjective = (initialEnemyCount > 0) && (enemiesKilled >= initialEnemyCount / 2);
    
    std::string treasureStatus = treasureObjective ? "[DONE]" : "[NEED MORE]";
    std::string enemyStatus = enemyObjective ? "[DONE]" : "[NEED MORE]";
    
    drawSimpleText(window, treasureStatus, WINDOW_WIDTH - 100, 105);
    drawSimpleText(window, enemyStatus, WINDOW_WIDTH - 100, 120);
    
    // Attack instructions and player stats
    sf::RectangleShape controlsBg(sf::Vector2f(250, 100));
    controlsBg.setPosition(10, 90);
    controlsBg.setFillColor(sf::Color(50, 50, 50, 180));
    controlsBg.setOutlineThickness(2);
    controlsBg.setOutlineColor(sf::Color::White);
    window.draw(controlsBg);
    
    drawSimpleText(window, "CONTROLS:", 20, 100);
    drawSimpleText(window, "WASD - Move", 20, 115);
    drawSimpleText(window, "SPACE - Attack (near enemies)", 20, 130);
    drawSimpleText(window, "ESC - Pause Menu", 20, 145);
    
    // Player attack power and status
    std::string attackText = "Attack: " + std::to_string(player->getEffectiveAttack());
    drawSimpleText(window, attackText, 20, 165);
    
    // Attack readiness indicator
    if (player->getIsAttacking()) {
        drawSimpleText(window, "ATTACKING!", 150, 165);
    } else {
        drawSimpleText(window, "Ready to attack", 150, 165);
    }
}

bool Game::checkWallCollision(sf::Vector2f position, sf::Vector2f size) {
    if (!dungeon) return false;
    
    // Check all four corners of the player
    sf::Vector2f topLeft = position - size / 2.0f;
    sf::Vector2f topRight = sf::Vector2f(position.x + size.x / 2.0f, position.y - size.y / 2.0f);
    sf::Vector2f bottomLeft = sf::Vector2f(position.x - size.x / 2.0f, position.y + size.y / 2.0f);
    sf::Vector2f bottomRight = position + size / 2.0f;
    
    return dungeon->isWall(topLeft.x, topLeft.y) ||
           dungeon->isWall(topRight.x, topRight.y) ||
           dungeon->isWall(bottomLeft.x, bottomLeft.y) ||
           dungeon->isWall(bottomRight.x, bottomRight.y);
}

void Game::transitionToState(GameState newState) {
    if (transitionManager) {
        transitionManager->startTransition(currentState, newState);
    }
}

void Game::renderWelcomeScreen() {
    // Refined gradient background with subtle depth
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(20, 30, 45));
    window.draw(background);
    
    // Add subtle gradient layers for depth
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape gradientLayer(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 5));
        int alpha = 20 + i * 10;
        gradientLayer.setFillColor(sf::Color(25 + i * 5, 35 + i * 5, 50 + i * 5, alpha));
        gradientLayer.setPosition(0, i * WINDOW_HEIGHT / 5);
        window.draw(gradientLayer);
    }
    
    // Add animated decorative elements
    static float animationTime = 0.0f;
    animationTime += 0.02f;
    
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 4));
    decorTop.setFillColor(sf::Color(100 + 50 * std::sin(animationTime), 150, 200));
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 4));
    decorBottom.setFillColor(sf::Color(100 + 50 * std::sin(animationTime + 3.14f), 150, 200));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 4);
    window.draw(decorBottom);
    
    // Main welcome panel
    sf::RectangleShape welcomePanel(sf::Vector2f(600, 500));
    welcomePanel.setFillColor(sf::Color(30, 40, 70));
    welcomePanel.setPosition(WINDOW_WIDTH/2 - 300, 100);
    welcomePanel.setOutlineThickness(4);
    welcomePanel.setOutlineColor(sf::Color(80, 150, 200));
    window.draw(welcomePanel);
    
    // Title with enhanced styling and glow effect
    sf::RectangleShape titleGlow(sf::Vector2f(550, 80));
    titleGlow.setFillColor(sf::Color(50, 100, 150, 100));
    titleGlow.setPosition(WINDOW_WIDTH/2 - 275, 115);
    window.draw(titleGlow);
    
    sf::RectangleShape titleBg(sf::Vector2f(520, 70));
    titleBg.setFillColor(sf::Color(60, 120, 180));
    titleBg.setPosition(WINDOW_WIDTH/2 - 260, 120);
    titleBg.setOutlineThickness(3);
    titleBg.setOutlineColor(sf::Color::White);
    window.draw(titleBg);
    
    // Title with multiple shadow layers for depth - properly centered
    float titleTextWidth = strlen("DUNGEON CRAWLER") * 7;
    float titleX = WINDOW_WIDTH/2 - titleTextWidth/2;
    drawSimpleText(window, "DUNGEON CRAWLER", titleX + 2, 148);
    drawSimpleText(window, "DUNGEON CRAWLER", titleX + 1, 147);
    drawSimpleText(window, "DUNGEON CRAWLER", titleX, 146);
    
    // Subtitle with enhanced styling
    sf::RectangleShape subtitleBg(sf::Vector2f(350, 50));
    subtitleBg.setFillColor(sf::Color(90, 140, 190));
    subtitleBg.setPosition(WINDOW_WIDTH/2 - 175, 220);
    subtitleBg.setOutlineThickness(2);
    subtitleBg.setOutlineColor(sf::Color::White);
    window.draw(subtitleBg);
    
    float welcomeTextWidth = strlen("WELCOME") * 7;
    float welcomeX = WINDOW_WIDTH/2 - welcomeTextWidth/2;
    drawSimpleText(window, "WELCOME", welcomeX + 1, 241);
    drawSimpleText(window, "WELCOME", welcomeX, 240);
    
    // Enhanced buttons with better spacing
    drawEnhancedButton(window, "LOGIN", WINDOW_WIDTH/2 - 120, 320, 240, 60, 
                      sf::Color(40, 180, 40), sf::Color(80, 220, 80));
    drawEnhancedButton(window, "REGISTER", WINDOW_WIDTH/2 - 120, 410, 240, 60, 
                      sf::Color(40, 80, 180), sf::Color(80, 120, 220));
}

void Game::renderLoginScreen() {
    // Enhanced gradient-like background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(25, 35, 70));
    window.draw(background);
    
    // Add decorative elements
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 3));
    decorTop.setFillColor(sf::Color(100, 200, 100));
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 3));
    decorBottom.setFillColor(sf::Color(100, 200, 100));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 3);
    window.draw(decorBottom);
    
    // Main login panel
    sf::RectangleShape loginPanel(sf::Vector2f(500, 420));
    loginPanel.setFillColor(sf::Color(40, 50, 90));
    loginPanel.setPosition(WINDOW_WIDTH/2 - 250, 120);
    loginPanel.setOutlineThickness(3);
    loginPanel.setOutlineColor(sf::Color(80, 120, 200));
    window.draw(loginPanel);
    
    // Title with enhanced styling
    sf::RectangleShape titleBg(sf::Vector2f(300, 60));
    titleBg.setFillColor(sf::Color(60, 180, 60));
    titleBg.setPosition(WINDOW_WIDTH/2 - 150, 140);
    titleBg.setOutlineThickness(2);
    titleBg.setOutlineColor(sf::Color::White);
    window.draw(titleBg);
    
    // Title shadow effect - properly centered
    float loginTextWidth = strlen("LOGIN") * 7;
    float loginX = WINDOW_WIDTH/2 - loginTextWidth/2;
    drawSimpleText(window, "LOGIN", loginX + 1, 166);
    drawSimpleText(window, "LOGIN", loginX, 165);
    
    // Enhanced input fields with better labels
    drawEnhancedInputField(window, "USERNAME", username, WINDOW_WIDTH/2 - 200, 240, 
                          currentInputField == InputField::USERNAME);
    drawEnhancedInputField(window, "PASSWORD", password, WINDOW_WIDTH/2 - 200, 320, 
                          currentInputField == InputField::PASSWORD, true);
    
    // Enhanced buttons with better styling
    drawEnhancedButton(window, "LOGIN", WINDOW_WIDTH/2 - 100, 400, 200, 50, 
                      sf::Color(40, 180, 40), sf::Color(60, 220, 60));
    drawEnhancedButton(window, "BACK", WINDOW_WIDTH/2 - 100, 470, 200, 50, 
                      sf::Color(180, 40, 40), sf::Color(220, 60, 60));
}

void Game::renderRegisterScreen() {
    // Enhanced gradient-like background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(35, 25, 60));
    window.draw(background);
    
    // Add decorative elements
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 3));
    decorTop.setFillColor(sf::Color(100, 100, 200));
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 3));
    decorBottom.setFillColor(sf::Color(100, 100, 200));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 3);
    window.draw(decorBottom);
    
    // Main register panel
    sf::RectangleShape registerPanel(sf::Vector2f(500, 420));
    registerPanel.setFillColor(sf::Color(50, 40, 80));
    registerPanel.setPosition(WINDOW_WIDTH/2 - 250, 120);
    registerPanel.setOutlineThickness(3);
    registerPanel.setOutlineColor(sf::Color(120, 80, 200));
    window.draw(registerPanel);
    
    // Title with enhanced styling
    sf::RectangleShape titleBg(sf::Vector2f(350, 60));
    titleBg.setFillColor(sf::Color(80, 60, 180));
    titleBg.setPosition(WINDOW_WIDTH/2 - 175, 140);
    titleBg.setOutlineThickness(2);
    titleBg.setOutlineColor(sf::Color::White);
    window.draw(titleBg);
    
    // Title with shadow effect - properly centered
    float registerTextWidth = strlen("REGISTER") * 7;
    float registerX = WINDOW_WIDTH/2 - registerTextWidth/2;
    drawSimpleText(window, "REGISTER", registerX + 1, 166);
    drawSimpleText(window, "REGISTER", registerX, 165);
    
    // Enhanced input fields
    drawEnhancedInputField(window, "USERNAME", username, WINDOW_WIDTH/2 - 200, 240, 
                          currentInputField == InputField::USERNAME);
    drawEnhancedInputField(window, "PASSWORD", password, WINDOW_WIDTH/2 - 200, 320, 
                          currentInputField == InputField::PASSWORD, true);
    
    // Enhanced buttons
    drawEnhancedButton(window, "REGISTER", WINDOW_WIDTH/2 - 100, 400, 200, 50, 
                      sf::Color(60, 40, 180), sf::Color(100, 80, 220));
    drawEnhancedButton(window, "BACK", WINDOW_WIDTH/2 - 100, 470, 200, 50, 
                      sf::Color(180, 40, 40), sf::Color(220, 60, 60));
}

void Game::renderPlayerNameScreen() {
    // Enhanced gradient-like background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(45, 35, 65));
    window.draw(background);
    
    // Add decorative elements
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 3));
    decorTop.setFillColor(sf::Color(200, 150, 100));
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 3));
    decorBottom.setFillColor(sf::Color(200, 150, 100));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 3);
    window.draw(decorBottom);
    
    // Main player name panel
    sf::RectangleShape namePanel(sf::Vector2f(500, 350));
    namePanel.setFillColor(sf::Color(60, 50, 90));
    namePanel.setPosition(WINDOW_WIDTH/2 - 250, 150);
    namePanel.setOutlineThickness(3);
    namePanel.setOutlineColor(sf::Color(200, 150, 100));
    window.draw(namePanel);
    
    // Title with enhanced styling
    sf::RectangleShape titleBg(sf::Vector2f(450, 60));
    titleBg.setFillColor(sf::Color(150, 100, 80));
    titleBg.setPosition(WINDOW_WIDTH/2 - 225, 170);
    titleBg.setOutlineThickness(2);
    titleBg.setOutlineColor(sf::Color::White);
    window.draw(titleBg);
    
    // Title with shadow effect - properly centered
    float playerNameTextWidth = strlen("ENTER PLAYER NAME") * 7;
    float playerNameX = WINDOW_WIDTH/2 - playerNameTextWidth/2;
    drawSimpleText(window, "ENTER PLAYER NAME", playerNameX + 1, 196);
    drawSimpleText(window, "ENTER PLAYER NAME", playerNameX, 195);
    
    // Enhanced input field
    drawEnhancedInputField(window, "PLAYER NAME", playerName, WINDOW_WIDTH/2 - 200, 280, 
                          currentInputField == InputField::PLAYER_NAME);
    
    // Enhanced start button
    drawEnhancedButton(window, "START GAME", WINDOW_WIDTH/2 - 100, 360, 200, 50, 
                      sf::Color(180, 120, 40), sf::Color(220, 160, 80));
}

void Game::drawInputField(sf::RenderWindow& window, const std::string& label, const std::string& text, 
                         float x, float y, bool isActive, bool isPassword) {
    // Label background
    sf::RectangleShape labelBg(sf::Vector2f(label.length() * 10 + 20, 25));
    labelBg.setFillColor(sf::Color(80, 80, 80));
    labelBg.setPosition(x, y - 30);
    labelBg.setOutlineThickness(1);
    labelBg.setOutlineColor(sf::Color::White);
    window.draw(labelBg);
    
    // Simple label placeholder
    sf::RectangleShape labelText(sf::Vector2f(labelBg.getSize().x * 0.8f, 12));
    labelText.setFillColor(sf::Color::White);
    labelText.setPosition(x + 10, y - 24);
    window.draw(labelText);
    
    // Input field background
    sf::RectangleShape fieldBg(sf::Vector2f(400, 35));
    fieldBg.setFillColor(isActive ? sf::Color(80, 80, 120) : sf::Color(50, 50, 50));
    fieldBg.setPosition(x, y);
    fieldBg.setOutlineThickness(2);
    fieldBg.setOutlineColor(isActive ? sf::Color::Yellow : sf::Color(128, 128, 128));
    window.draw(fieldBg);
    
    // Display text (masked if password)
    std::string displayText = text;
    if (isPassword) {
        displayText = std::string(text.length(), '*');
    }
    
    // Draw input text as a simple representation
    if (!displayText.empty()) {
        sf::RectangleShape textRepresentation(sf::Vector2f(displayText.length() * 8, 16));
        textRepresentation.setFillColor(sf::Color::White);
        textRepresentation.setPosition(x + 8, y + 10);
        window.draw(textRepresentation);
    }
    
    // Cursor (blinking effect)
    if (isActive) {
        static float cursorTimer = 0.0f;
        cursorTimer += 0.016f; // Approximate frame time
        if (fmod(cursorTimer, 1.0f) < 0.5f) { // Blink every half second
            sf::RectangleShape cursor(sf::Vector2f(2, 20));
            cursor.setFillColor(sf::Color::White);
            cursor.setPosition(x + 8 + text.length() * 10, y + 8);
            window.draw(cursor);
        }
    }
}

void Game::drawButton(sf::RenderWindow& window, const std::string& text, float x, float y, 
                     float width, float height, sf::Color color) {
    // Button background
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setFillColor(color);
    button.setPosition(x, y);
    button.setOutlineThickness(3);
    button.setOutlineColor(sf::Color::White);
    window.draw(button);
    
    // Draw button text centered
    float textX = x + (width - text.length() * 12) / 2;
    float textY = y + (height - 10) / 2;
    drawSimpleText(window, text, textX, textY);
}

void Game::handleTextInput(sf::Uint32 unicode) {
    if (!isTyping) return;
    
    std::string* targetString = nullptr;
    switch (currentInputField) {
        case InputField::USERNAME:
            targetString = &username;
            break;
        case InputField::PASSWORD:
            targetString = &password;
            break;
        case InputField::PLAYER_NAME:
            targetString = &playerName;
            break;
        default:
            return;
    }
    
    if (unicode == 8) { // Backspace
        if (!targetString->empty()) {
            targetString->pop_back();
        }
    } else if (unicode >= 32 && unicode < 127) { // Printable characters
        if (targetString->length() < 20) { // Limit length
            *targetString += static_cast<char>(unicode);
        }
    }
}

void Game::drawEnhancedInputField(sf::RenderWindow& window, const std::string& label, const std::string& text, 
                                  float x, float y, bool isActive, bool isPassword) {
    // Subtle label with gradient effect
    sf::RectangleShape labelBg(sf::Vector2f(120, 25));
    labelBg.setFillColor(sf::Color(75, 85, 110));
    labelBg.setPosition(x, y - 35);
    labelBg.setOutlineThickness(1);
    labelBg.setOutlineColor(sf::Color(95, 105, 130));
    window.draw(labelBg);
    
    // Label highlight for depth
    sf::RectangleShape labelHighlight(sf::Vector2f(116, 2));
    labelHighlight.setFillColor(sf::Color(100, 110, 135));
    labelHighlight.setPosition(x + 2, y - 33);
    window.draw(labelHighlight);
    
    // Draw label text
    drawSimpleText(window, label, x + 10, y - 30);
    
    // Input field with refined styling and subtle gradients
    sf::RectangleShape fieldBg(sf::Vector2f(400, 40));
    if (isActive) {
        fieldBg.setFillColor(sf::Color(95, 105, 125));
        fieldBg.setOutlineColor(sf::Color(120, 160, 200));
        fieldBg.setOutlineThickness(2);
    } else {
        fieldBg.setFillColor(sf::Color(65, 75, 95));
        fieldBg.setOutlineColor(sf::Color(85, 95, 115));
        fieldBg.setOutlineThickness(1);
    }
    fieldBg.setPosition(x, y);
    window.draw(fieldBg);
    
    // Subtle top highlight for depth
    sf::RectangleShape topHighlight(sf::Vector2f(398, 1));
    topHighlight.setFillColor(isActive ? sf::Color(110, 120, 140) : sf::Color(85, 95, 115));
    topHighlight.setPosition(x + 1, y + 1);
    window.draw(topHighlight);
    
    // Subtle inner shadow
    sf::RectangleShape innerShadow(sf::Vector2f(398, 2));
    innerShadow.setFillColor(sf::Color(45, 55, 75));
    innerShadow.setPosition(x + 1, y + 37);
    window.draw(innerShadow);
    
    // Display text
    std::string displayText = text;
    if (isPassword && !text.empty()) {
        displayText = std::string(text.length(), '*');
    }
    
    if (!displayText.empty()) {
        drawSimpleText(window, displayText, x + 12, y + 16);
    }
    
    // Refined cursor with smooth animation
    if (isActive) {
        static float cursorTimer = 0.0f;
        cursorTimer += 0.015f;
        float alpha = (std::sin(cursorTimer * 3.0f) + 1.0f) / 2.0f; // Smooth fade
        if (alpha > 0.3f) {
            sf::RectangleShape cursor(sf::Vector2f(2, 18));
            cursor.setFillColor(sf::Color(180, 200, 255, (sf::Uint8)(alpha * 255)));
            cursor.setPosition(x + 14 + displayText.length() * 7, y + 11);
            window.draw(cursor);
        }
    }
}

void Game::drawEnhancedButton(sf::RenderWindow& window, const std::string& text, float x, float y, 
                             float width, float height, sf::Color baseColor, sf::Color highlightColor) {
    // Subtle drop shadow with multiple layers for depth
    sf::RectangleShape shadow3(sf::Vector2f(width + 6, height + 6));
    shadow3.setFillColor(sf::Color(0, 0, 0, 40));
    shadow3.setPosition(x + 3, y + 3);
    window.draw(shadow3);
    
    sf::RectangleShape shadow2(sf::Vector2f(width + 3, height + 3));
    shadow2.setFillColor(sf::Color(0, 0, 0, 60));
    shadow2.setPosition(x + 2, y + 2);
    window.draw(shadow2);
    
    sf::RectangleShape shadow1(sf::Vector2f(width + 1, height + 1));
    shadow1.setFillColor(sf::Color(0, 0, 0, 80));
    shadow1.setPosition(x + 1, y + 1);
    window.draw(shadow1);
    
    // Main button with refined colors
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setFillColor(baseColor);
    button.setPosition(x, y);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color(baseColor.r + 40, baseColor.g + 40, baseColor.b + 40));
    window.draw(button);
    
    // Subtle top highlight for 3D effect
    sf::RectangleShape topHighlight(sf::Vector2f(width - 4, 3));
    topHighlight.setFillColor(highlightColor);
    topHighlight.setPosition(x + 2, y + 2);
    window.draw(topHighlight);
    
    // Side highlights for more depth
    sf::RectangleShape leftHighlight(sf::Vector2f(2, height - 8));
    leftHighlight.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, 150));
    leftHighlight.setPosition(x + 2, y + 4);
    window.draw(leftHighlight);
    
    // Bottom shadow for inset effect
    sf::RectangleShape bottomShadow(sf::Vector2f(width - 4, 2));
    bottomShadow.setFillColor(sf::Color(baseColor.r - 30, baseColor.g - 30, baseColor.b - 30));
    bottomShadow.setPosition(x + 2, y + height - 4);
    window.draw(bottomShadow);
    
    // Button text with shadow - properly centered
    float textWidth = text.length() * 7;
    float textX = x + (width - textWidth) / 2;
    float textY = y + (height - 7) / 2;
    
    // Text shadow
    drawSimpleText(window, text, textX + 1, textY + 1);
    // Main text
    drawSimpleText(window, text, textX, textY);
}

void Game::renderGameOverScreen() {
    // Dark, somber background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(20, 20, 30));
    window.draw(background);
    
    // Add subtle dark borders
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 4));
    decorTop.setFillColor(sf::Color(80, 40, 40));
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 4));
    decorBottom.setFillColor(sf::Color(80, 40, 40));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 4);
    window.draw(decorBottom);
    
    // Main game over panel
    sf::RectangleShape gameOverPanel(sf::Vector2f(600, 450));
    gameOverPanel.setFillColor(sf::Color(40, 30, 40));
    gameOverPanel.setPosition(WINDOW_WIDTH/2 - 300, 175);
    gameOverPanel.setOutlineThickness(3);
    gameOverPanel.setOutlineColor(sf::Color(120, 80, 80));
    window.draw(gameOverPanel);
    
    // Game Over title
    sf::RectangleShape titleBg(sf::Vector2f(500, 80));
    titleBg.setFillColor(sf::Color(100, 40, 40));
    titleBg.setPosition(WINDOW_WIDTH/2 - 250, 200);
    titleBg.setOutlineThickness(3);
    titleBg.setOutlineColor(sf::Color(160, 80, 80));
    window.draw(titleBg);
    
    float gameOverTextWidth = strlen("GAME OVER") * 7;
    float gameOverX = WINDOW_WIDTH/2 - gameOverTextWidth/2;
    drawSimpleText(window, "GAME OVER", gameOverX + 1, 236);
    drawSimpleText(window, "GAME OVER", gameOverX, 235);
    
    // Stats display
    sf::RectangleShape statsBg(sf::Vector2f(500, 180));
    statsBg.setFillColor(sf::Color(50, 40, 50));
    statsBg.setPosition(WINDOW_WIDTH/2 - 250, 320);
    statsBg.setOutlineThickness(2);
    statsBg.setOutlineColor(sf::Color(100, 80, 100));
    window.draw(statsBg);
    
    // Final stats
    std::string levelText = "REACHED LEVEL " + std::to_string(currentLevel);
    float levelTextWidth = levelText.length() * 7;
    float levelX = WINDOW_WIDTH/2 - levelTextWidth/2;
    drawSimpleText(window, levelText, levelX, 340);
    
    std::string scoreText = "FINAL SCORE: " + std::to_string(score);
    float scoreTextWidth = scoreText.length() * 7;
    float scoreX = WINDOW_WIDTH/2 - scoreTextWidth/2;
    drawSimpleText(window, scoreText, scoreX, 360);
    
    std::string treasureText = "TREASURES: " + std::to_string(treasuresCollected);
    float treasureTextWidth = treasureText.length() * 7;
    float treasureX = WINDOW_WIDTH/2 - treasureTextWidth/2;
    drawSimpleText(window, treasureText, treasureX, 380);
    
    std::string enemyText = "ENEMIES: " + std::to_string(enemiesKilled);
    float enemyTextWidth = enemyText.length() * 7;
    float enemyX = WINDOW_WIDTH/2 - enemyTextWidth/2;
    drawSimpleText(window, enemyText, enemyX, 400);
    
    // Instructions
    std::string instructText = "PRESS R TO RESTART OR ESC FOR MENU";
    float instructTextWidth = instructText.length() * 7;
    float instructX = WINDOW_WIDTH/2 - instructTextWidth/2;
    drawSimpleText(window, instructText, instructX, 440);
    
    // Action buttons
    drawEnhancedButton(window, "RESTART", WINDOW_WIDTH/2 - 120, 550, 100, 45, 
                      sf::Color(60, 100, 60), sf::Color(80, 140, 80));
    drawEnhancedButton(window, "MENU", WINDOW_WIDTH/2 + 20, 550, 100, 45, 
                      sf::Color(100, 60, 60), sf::Color(140, 80, 80));
}

void Game::renderPauseScreen() {
    // Render game world first (dimmed)
    if (camera) {
        camera->setView(window);
    }
    
    if (dungeon) {
        dungeon->render(window, camera ? camera->getView() : window.getDefaultView());
    }
    
    if (player) {
        player->render(window);
    }
    
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->render(window);
        }
    }
    
    // Reset to default view for UI
    window.setView(window.getDefaultView());
    
    // Semi-transparent overlay
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    // Main pause menu panel
    sf::RectangleShape pausePanel(sf::Vector2f(400, 350));
    pausePanel.setFillColor(sf::Color(40, 60, 80, 240));
    pausePanel.setPosition(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 175);
    pausePanel.setOutlineThickness(3);
    pausePanel.setOutlineColor(sf::Color(120, 180, 220));
    window.draw(pausePanel);
    
    // Pause title
    sf::RectangleShape titleBg(sf::Vector2f(300, 50));
    titleBg.setFillColor(sf::Color(60, 100, 140));
    titleBg.setPosition(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 150);
    titleBg.setOutlineThickness(2);
    titleBg.setOutlineColor(sf::Color::White);
    window.draw(titleBg);
    
    // Pause title text
    float pauseTextWidth = strlen("GAME PAUSED") * 7;
    float pauseX = WINDOW_WIDTH/2 - pauseTextWidth/2;
    drawSimpleText(window, "GAME PAUSED", pauseX + 1, WINDOW_HEIGHT/2 - 134);
    drawSimpleText(window, "GAME PAUSED", pauseX, WINDOW_HEIGHT/2 - 135);
    
    // Menu buttons with keyboard shortcut indicators
    float buttonY = WINDOW_HEIGHT/2 - 80;
    drawEnhancedButton(window, "RESUME (ESC)", WINDOW_WIDTH/2 - 100, buttonY, 200, 50, 
                      sf::Color(40, 140, 40), sf::Color(80, 180, 80));
    
    drawEnhancedButton(window, "RESTART (R)", WINDOW_WIDTH/2 - 100, buttonY + 70, 200, 50, 
                      sf::Color(140, 100, 40), sf::Color(180, 140, 80));
    
    drawEnhancedButton(window, "QUIT TO MENU (Q)", WINDOW_WIDTH/2 - 100, buttonY + 140, 200, 50, 
                      sf::Color(140, 40, 40), sf::Color(180, 80, 80));
}

void Game::renderVictoryScreen() {
    // Enhanced victory background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(20, 40, 20));
    window.draw(background);
    
    // Add celebratory decorative elements
    sf::RectangleShape decorTop(sf::Vector2f(WINDOW_WIDTH, 5));
    decorTop.setFillColor(sf::Color(255, 215, 0)); // Gold
    decorTop.setPosition(0, 0);
    window.draw(decorTop);
    
    sf::RectangleShape decorBottom(sf::Vector2f(WINDOW_WIDTH, 5));
    decorBottom.setFillColor(sf::Color(255, 215, 0));
    decorBottom.setPosition(0, WINDOW_HEIGHT - 5);
    window.draw(decorBottom);
    
    // Main victory panel
    sf::RectangleShape victoryPanel(sf::Vector2f(600, 500));
    victoryPanel.setFillColor(sf::Color(40, 70, 40));
    victoryPanel.setPosition(WINDOW_WIDTH/2 - 300, 150);
    victoryPanel.setOutlineThickness(4);
    victoryPanel.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(victoryPanel);
    
    // Victory title
    sf::RectangleShape titleBg(sf::Vector2f(500, 80));
    titleBg.setFillColor(sf::Color(60, 150, 60));
    titleBg.setPosition(WINDOW_WIDTH/2 - 250, 180);
    titleBg.setOutlineThickness(3);
    titleBg.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(titleBg);
    
    float victoryTextWidth = strlen("LEVEL COMPLETE!") * 7;
    float victoryX = WINDOW_WIDTH/2 - victoryTextWidth/2;
    drawSimpleText(window, "LEVEL COMPLETE!", victoryX + 1, 216);
    drawSimpleText(window, "LEVEL COMPLETE!", victoryX, 215);
    
    // Stats display
    sf::RectangleShape statsBg(sf::Vector2f(500, 200));
    statsBg.setFillColor(sf::Color(50, 80, 50));
    statsBg.setPosition(WINDOW_WIDTH/2 - 250, 300);
    statsBg.setOutlineThickness(2);
    statsBg.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(statsBg);
    
    // Level info
    std::string levelText = "LEVEL " + std::to_string(currentLevel - 1) + " COMPLETED";
    float levelTextWidth = levelText.length() * 7;
    float levelX = WINDOW_WIDTH/2 - levelTextWidth/2;
    drawSimpleText(window, levelText, levelX, 320);
    
    // Score info
    std::string scoreText = "SCORE: " + std::to_string(score);
    float scoreTextWidth = scoreText.length() * 7;
    float scoreX = WINDOW_WIDTH/2 - scoreTextWidth/2;
    drawSimpleText(window, scoreText, scoreX, 340);
    
    // Treasures info
    std::string treasureText = "TREASURES: " + std::to_string(treasuresCollected);
    float treasureTextWidth = treasureText.length() * 7;
    float treasureX = WINDOW_WIDTH/2 - treasureTextWidth/2;
    drawSimpleText(window, treasureText, treasureX, 360);
    
    // Enemies info
    std::string enemyText = "ENEMIES DEFEATED: " + std::to_string(enemiesKilled);
    float enemyTextWidth = enemyText.length() * 7;
    float enemyX = WINDOW_WIDTH/2 - enemyTextWidth/2;
    drawSimpleText(window, enemyText, enemyX, 380);
    
    // Next level info
    std::string nextLevelText = "NEXT: LEVEL " + std::to_string(currentLevel);
    float nextLevelTextWidth = nextLevelText.length() * 7;
    float nextLevelX = WINDOW_WIDTH/2 - nextLevelTextWidth/2;
    drawSimpleText(window, nextLevelText, nextLevelX, 420);
    
    // Continue button
    drawEnhancedButton(window, "CONTINUE", WINDOW_WIDTH/2 - 100, 550, 200, 50, 
                      sf::Color(40, 180, 40), sf::Color(80, 220, 80));
    
    // Menu button
    drawEnhancedButton(window, "MAIN MENU", WINDOW_WIDTH/2 - 100, 620, 200, 50, 
                      sf::Color(180, 180, 40), sf::Color(220, 220, 80));
}

void Game::drawSimpleText(sf::RenderWindow& window, const std::string& text, float x, float y) {
    // Create simple pixel-based font system with 5x7 bitmap patterns
    std::map<char, std::vector<std::string>> letterPatterns = {
        {'A', {"  *  ", " * * ", "*   *", "*****", "*   *", "*   *", "     "}},
        {'B', {"**** ", "*   *", "**** ", "**** ", "*   *", "**** ", "     "}},
        {'C', {" ****", "*    ", "*    ", "*    ", "*    ", " ****", "     "}},
        {'D', {"**** ", "*   *", "*   *", "*   *", "*   *", "**** ", "     "}},
        {'E', {"*****", "*    ", "**** ", "**** ", "*    ", "*****", "     "}},
        {'F', {"*****", "*    ", "**** ", "**** ", "*    ", "*    ", "     "}},
        {'G', {" ****", "*    ", "* ***", "*   *", "*   *", " ****", "     "}},
        {'H', {"*   *", "*   *", "*****", "*****", "*   *", "*   *", "     "}},
        {'I', {"*****", "  *  ", "  *  ", "  *  ", "  *  ", "*****", "     "}},
        {'J', {"*****", "    *", "    *", "    *", "*   *", " ****", "     "}},
        {'K', {"*   *", "*  * ", "*** ", "*** ", "*  * ", "*   *", "     "}},
        {'L', {"*    ", "*    ", "*    ", "*    ", "*    ", "*****", "     "}},
        {'M', {"*   *", "** **", "* * *", "*   *", "*   *", "*   *", "     "}},
        {'N', {"*   *", "**  *", "* * *", "*  **", "*   *", "*   *", "     "}},
        {'O', {" *** ", "*   *", "*   *", "*   *", "*   *", " *** ", "     "}},
        {'P', {"**** ", "*   *", "**** ", "*    ", "*    ", "*    ", "     "}},
        {'Q', {" *** ", "*   *", "*   *", "* * *", "*  **", " ****", "     "}},
        {'R', {"**** ", "*   *", "**** ", "*** ", "*  * ", "*   *", "     "}},
        {'S', {" ****", "*    ", " *** ", "    *", "    *", "**** ", "     "}},
        {'T', {"*****", "  *  ", "  *  ", "  *  ", "  *  ", "  *  ", "     "}},
        {'U', {"*   *", "*   *", "*   *", "*   *", "*   *", " *** ", "     "}},
        {'V', {"*   *", "*   *", "*   *", "*   *", " * * ", "  *  ", "     "}},
        {'W', {"*   *", "*   *", "*   *", "* * *", "** **", "*   *", "     "}},
        {'X', {"*   *", " * * ", "  *  ", "  *  ", " * * ", "*   *", "     "}},
        {'Y', {"*   *", " * * ", "  *  ", "  *  ", "  *  ", "  *  ", "     "}},
        {'Z', {"*****", "   * ", "  *  ", " *   ", "*    ", "*****", "     "}},
        {' ', {"     ", "     ", "     ", "     ", "     ", "     ", "     "}},
        {'0', {" *** ", "*   *", "*  **", "* * *", "**  *", " *** ", "     "}},
        {'1', {"  *  ", " **  ", "  *  ", "  *  ", "  *  ", "*****", "     "}},
        {'2', {" *** ", "*   *", "   * ", "  *  ", " *   ", "*****", "     "}},
        {'3', {" *** ", "*   *", "  ** ", "   * ", "*   *", " *** ", "     "}},
        {'4', {"*   *", "*   *", "*****", "    *", "    *", "    *", "     "}},
        {'5', {"*****", "*    ", "**** ", "    *", "*   *", " *** ", "     "}},
        {'6', {" ****", "*    ", "**** ", "*   *", "*   *", " *** ", "     "}},
        {'7', {"*****", "    *", "   * ", "  *  ", " *   ", "*    ", "     "}},
        {'8', {" *** ", "*   *", " *** ", " *** ", "*   *", " *** ", "     "}},
        {'9', {" *** ", "*   *", "*   *", " ****", "    *", " *** ", "     "}},
        {':', {"     ", "  *  ", "     ", "     ", "  *  ", "     ", "     "}},
        {'*', {"     ", " * * ", "  *  ", "*****", "  *  ", " * * ", "     "}}
    };
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = std::toupper(text[i]);
        float charX = x + i * 7; // 6 pixels per char + 1 spacing
        
        if (letterPatterns.find(c) != letterPatterns.end()) {
            const auto& pattern = letterPatterns[c];
            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (row < (int)pattern.size() && col < (int)pattern[row].length() && pattern[row][col] == '*') {
                        sf::RectangleShape pixel(sf::Vector2f(1, 1));
                        pixel.setFillColor(sf::Color::White);
                        pixel.setPosition(charX + col, y + row);
                        window.draw(pixel);
                    }
                }
            }
        }
    }
}