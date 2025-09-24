#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

enum class EnemyType {
    GOBLIN,
    ORC,
    SKELETON
};

class Enemy {
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape sprite;
    sf::Color color;
    
    Stats stats;
    EnemyType type;
    float speed;
    float attackRange;
    float detectionRange;
    float attackCooldown;
    float lastAttackTime;
    
    // AI States
    enum class AIState {
        IDLE,
        PATROL,
        CHASE,
        ATTACK,
        DEAD
    } aiState;
    
    sf::Vector2f patrolTarget;
    float patrolTimer;
    
public:
    Enemy(EnemyType t, float x, float y);
    virtual ~Enemy() = default;
    
    virtual void update(float deltaTime, const Player& player);
    virtual void render(sf::RenderWindow& window);
    virtual void attack(Player& player);
    void takeDamage(int damage);
    void updateAI(float deltaTime, const Player& player);
    void moveTowards(sf::Vector2f target, float deltaTime);
    
    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
    bool isAlive() const { return stats.health > 0; }
    bool isDead() const { return aiState == AIState::DEAD; }
    EnemyType getType() const { return type; }
    int getExperienceReward() const;
    Stats getStats() const { return stats; }
    
    // Collision check
    bool isColliding(const sf::FloatRect& other) const;
    
protected:
    float distanceTo(sf::Vector2f target) const;
    sf::Vector2f normalize(sf::Vector2f vector) const;
    void generatePatrolTarget();
};