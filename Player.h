#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Stats {
    int health;
    int maxHealth;
    int attack;
    int defense;
    int experience;
    int level;
    
    Stats() : health(100), maxHealth(100), attack(15), defense(5), experience(0), level(1) {}
};

class Player {
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape sprite;
    sf::Color color;
    
    Stats stats;
    bool isAttacking;
    float attackCooldown;
    float speed;
    
    // Animation
    float animationTimer;
    int currentFrame;
    
    // Power-up effects
    struct ActivePowerUp {
        int damageBoost = 0;
        int speedBoost = 0;
        int armorBoost = 0;
        float damageBoostTimer = 0.0f;
        float speedBoostTimer = 0.0f;
        float armorBoostTimer = 0.0f;
    } activePowerUps;
    
public:
    Player(float x, float y);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void handleInput();
    void move(float dx, float dy);
    void attack();
    void takeDamage(int damage);
    void gainExperience(int exp);
    void levelUp();
    
    // Power-up methods
    void applyPowerUp(int powerUpType, int value, float duration);
    void updatePowerUps(float deltaTime);
    void heal(int amount);
    int getEffectiveAttack() const;
    int getEffectiveSpeed() const;
    int getEffectiveArmor() const;
    
    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
    Stats getStats() const { return stats; }
    bool isAlive() const { return stats.health > 0; }
    bool getIsAttacking() const { return isAttacking; }
    
    // Setters
    void setPosition(float x, float y);
    void setPosition(sf::Vector2f pos);
    
    static const float PLAYER_SIZE;
    static const float PLAYER_SPEED;
};