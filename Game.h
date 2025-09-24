#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Player.h"
#include "Dungeon.h"
#include "Camera.h"
#include "Enemy.h"
#include "GameState.h"

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    sf::Font font;
    
    std::unique_ptr<Player> player;
    std::unique_ptr<Dungeon> dungeon;
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<Enemy>> enemies;
    
    GameState currentState;
    int score;
    bool isRunning;
    
public:
    Game();
    ~Game();
    
    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void initializeGame();
    void resetGame();
    void generateLevel();
    void updateEnemies(float deltaTime);
    void renderUI();
    
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 800;
};