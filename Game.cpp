#include "Game.h"
#include <iostream>
#include <cmath>

const int Game::WINDOW_WIDTH;
const int Game::WINDOW_HEIGHT;

Game::Game() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dungeon Crawler", sf::Style::Close)
    , currentState(GameState::MENU)
    , score(0)
    , isRunning(true) {
    
    window.setFramerateLimit(60);
    
    // Try to load font (optional - will use default if fails)
    if (!font.loadFromFile("arial.ttf")) {
        // Use system default if available, or continue without font
        std::cout << "Warning: Could not load font file\n";
    }
    
    // Skip menu and go directly to playing for testing
    currentState = GameState::PLAYING;
    
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
                
            case sf::Event::KeyPressed:
                if (currentState == GameState::MENU) {
                    if (event.key.code == sf::Keyboard::Space) {
                        currentState = GameState::PLAYING;
                        resetGame();
                    }
                } else if (currentState == GameState::GAME_OVER) {
                    if (event.key.code == sf::Keyboard::R) {
                        currentState = GameState::PLAYING;
                        resetGame();
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        currentState = GameState::MENU;
                    }
                } else if (currentState == GameState::PLAYING) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentState = GameState::PAUSED;
                    }
                } else if (currentState == GameState::PAUSED) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentState = GameState::PLAYING;
                    }
                }
                break;
                
            default:
                break;
        }
    }
}

void Game::update(float deltaTime) {
    if (player && player->isAlive()) {
        player->update(deltaTime);
        
        // Check wall collisions with proper collision response
        sf::Vector2f newPos = player->getPosition();
        sf::FloatRect playerBounds = player->getBounds();
        
        // Check collision with walls
        bool collisionX = dungeon->isWall(newPos.x + playerBounds.width/2, newPos.y) || 
                         dungeon->isWall(newPos.x - playerBounds.width/2, newPos.y);
        bool collisionY = dungeon->isWall(newPos.x, newPos.y + playerBounds.height/2) || 
                         dungeon->isWall(newPos.x, newPos.y - playerBounds.height/2);
        
        if (collisionX || collisionY) {
            // Move player back to previous valid position
            sf::Vector2f prevPos = player->getPosition();
            if (collisionX && !collisionY) {
                player->setPosition(prevPos.x - player->getPosition().x + prevPos.x, newPos.y);
            } else if (!collisionX && collisionY) {
                player->setPosition(newPos.x, prevPos.y - player->getPosition().y + prevPos.y);
            } else {
                player->setPosition(prevPos);
            }
        }
        
        // Check treasure collection
        TileType currentTile = dungeon->getTileType(newPos.x, newPos.y);
        if (currentTile == TileType::TREASURE) {
            // Collect treasure
            score += 500;
            player->gainExperience(50);
            
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
        
        // Check if player is dead
        if (!player->isAlive()) {
            currentState = GameState::GAME_OVER;
        }
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0) { // Print every 60 frames
        std::cout << "Rendering frame " << frameCount << " in state " << (int)currentState << std::endl;
    }
    
    switch (currentState) {
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
            // Draw a test rectangle to ensure something is visible
            sf::RectangleShape testRect(sf::Vector2f(100, 100));
            testRect.setPosition(100, 100);
            testRect.setFillColor(sf::Color::Red);
            window.draw(testRect);
            
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
            
            // Reset to default view for UI
            window.setView(window.getDefaultView());
            renderUI();
            break;
        }
        
        case GameState::PAUSED: {
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
            
            // Dim overlay
            sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            overlay.setFillColor(sf::Color(0, 0, 0, 128));
            window.setView(window.getDefaultView());
            window.draw(overlay);
            
            sf::Text pauseText("PAUSED", font, 48);
            pauseText.setFillColor(sf::Color::White);
            pauseText.setPosition(WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 50);
            
            sf::Text instruction("Press ESC to Resume", font, 24);
            instruction.setFillColor(sf::Color::White);
            instruction.setPosition(WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20);
            
            window.draw(pauseText);
            window.draw(instruction);
            break;
        }
        
        case GameState::GAME_OVER: {
            sf::Text gameOverText("GAME OVER", font, 48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2 - 100);
            
            sf::Text scoreText("Score: " + std::to_string(score), font, 24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 - 20);
            
            sf::Text instruction("Press R to Restart, ESC for Menu", font, 20);
            instruction.setFillColor(sf::Color::White);
            instruction.setPosition(WINDOW_WIDTH/2 - 140, WINDOW_HEIGHT/2 + 40);
            
            window.draw(gameOverText);
            window.draw(scoreText);
            window.draw(instruction);
            break;
        }
        
        default:
            break;
    }
    
    window.display();
}

void Game::initializeGame() {
    std::cout << "Initializing game..." << std::endl;
    // Initialize game systems
    camera = std::make_unique<Camera>(WINDOW_WIDTH, WINDOW_HEIGHT);
    dungeon = std::make_unique<Dungeon>(50, 50);
    
    // Initialize the game immediately
    resetGame();
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::resetGame() {
    std::cout << "Resetting game..." << std::endl;
    
    // Generate new level
    generateLevel();
    
    // Reset score
    score = 0;
    
    // Create player at spawn point
    sf::Vector2f spawnPoint = dungeon->getPlayerSpawn();
    std::cout << "Player spawn: " << spawnPoint.x << ", " << spawnPoint.y << std::endl;
    player = std::make_unique<Player>(spawnPoint.x, spawnPoint.y);
    
    // Clear and regenerate enemies
    enemies.clear();
    std::vector<sf::Vector2f> enemySpawns = dungeon->getEnemySpawns(8);
    std::cout << "Generated " << enemySpawns.size() << " enemy spawns" << std::endl;
    for (const auto& spawn : enemySpawns) {
        // Randomly choose enemy type
        EnemyType type = static_cast<EnemyType>(rand() % 3);
        enemies.push_back(std::make_unique<Enemy>(type, spawn.x, spawn.y));
    }
}

void Game::generateLevel() {
    if (dungeon) {
        dungeon->generate();
    }
}

void Game::updateEnemies(float deltaTime) {
    for (auto it = enemies.begin(); it != enemies.end();) {
        if ((*it)->isDead()) {
            // Award experience for killing enemy
            if (player) {
                player->gainExperience((*it)->getExperienceReward());
                score += 100;
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
                    (*it)->takeDamage(player->getStats().attack);
                    
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
}