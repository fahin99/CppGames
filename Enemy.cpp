#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <random>

Enemy::Enemy(EnemyType t, float x, float y) 
    : position(x, y)
    , velocity(0, 0)
    , type(t)
    , aiState(AIState::PATROL)
    , attackCooldown(0.0f)
    , lastAttackTime(0.0f)
    , patrolTimer(0.0f) {
    
    // Set stats based on enemy type
    switch (type) {
        case EnemyType::GOBLIN:
            stats.health = stats.maxHealth = 30;
            stats.attack = 8;
            stats.defense = 2;
            speed = 80.0f;
            attackRange = 35.0f;
            detectionRange = 100.0f;
            color = sf::Color::Green;
            attackCooldown = 1.5f;
            break;
            
        case EnemyType::ORC:
            stats.health = stats.maxHealth = 60;
            stats.attack = 15;
            stats.defense = 5;
            speed = 60.0f;
            attackRange = 40.0f;
            detectionRange = 120.0f;
            color = sf::Color(128, 64, 0); // Brown
            attackCooldown = 2.0f;
            break;
            
        case EnemyType::SKELETON:
            stats.health = stats.maxHealth = 25;
            stats.attack = 12;
            stats.defense = 1;
            speed = 100.0f;
            attackRange = 30.0f;
            detectionRange = 150.0f;
            color = sf::Color(200, 200, 200); // Light gray
            attackCooldown = 1.0f;
            break;
    }
    
    // Initialize sprite
    sprite.setSize(sf::Vector2f(24.0f, 24.0f));
    sprite.setPosition(position);
    sprite.setFillColor(color);
    sprite.setOrigin(12.0f, 12.0f);
    
    // Generate initial patrol target
    generatePatrolTarget();
}

void Enemy::update(float deltaTime, const Player& player) {
    if (aiState == AIState::DEAD) return;
    
    // Update timers
    if (lastAttackTime > 0) {
        lastAttackTime -= deltaTime;
    }
    
    // Update AI behavior
    updateAI(deltaTime, player);
    
    // Apply movement
    position += velocity * deltaTime;
    sprite.setPosition(position);
    
    // Reset velocity for next frame
    velocity = sf::Vector2f(0, 0);
}

void Enemy::render(sf::RenderWindow& window) {
    if (aiState == AIState::DEAD) return;
    
    // Draw health bar above enemy
    if (stats.health < stats.maxHealth) {
        sf::RectangleShape healthBarBg(sf::Vector2f(30, 4));
        healthBarBg.setPosition(position.x - 15, position.y - 20);
        healthBarBg.setFillColor(sf::Color::Red);
        
        sf::RectangleShape healthBar(sf::Vector2f(30 * stats.health / stats.maxHealth, 4));
        healthBar.setPosition(position.x - 15, position.y - 20);
        healthBar.setFillColor(sf::Color::Green);
        
        window.draw(healthBarBg);
        window.draw(healthBar);
    }
    
    // Draw detection range (debug)
    if (aiState == AIState::CHASE) {
        sf::CircleShape detectionCircle(detectionRange);
        detectionCircle.setFillColor(sf::Color(255, 0, 0, 20));
        detectionCircle.setPosition(position.x - detectionRange, position.y - detectionRange);
        window.draw(detectionCircle);
    }
    
    window.draw(sprite);
}

void Enemy::attack(Player& player) {
    if (lastAttackTime <= 0 && distanceTo(player.getPosition()) <= attackRange) {
        // Perform attack
        player.takeDamage(stats.attack);
        lastAttackTime = attackCooldown;
        
        // Visual feedback
        sprite.setFillColor(sf::Color::Red);
        
        std::cout << "Enemy attacked player for " << stats.attack << " damage!" << std::endl;
    }
}

void Enemy::takeDamage(int damage) {
    int actualDamage = std::max(1, damage - stats.defense);
    stats.health = std::max(0, stats.health - actualDamage);
    
    // Visual feedback
    sprite.setFillColor(sf::Color::White);
    
    // Switch to chase state when damaged
    if (aiState != AIState::DEAD) {
        aiState = AIState::CHASE;
    }
    
    // Check if dead
    if (stats.health <= 0) {
        aiState = AIState::DEAD;
        sprite.setFillColor(sf::Color(100, 100, 100)); // Gray for dead
    }
    
    std::cout << "Enemy took " << actualDamage << " damage! Health: " << stats.health << "/" << stats.maxHealth << std::endl;
}

void Enemy::updateAI(float deltaTime, const Player& player) {
    float distanceToPlayer = distanceTo(player.getPosition());
    
    switch (aiState) {
        case AIState::IDLE:
            patrolTimer += deltaTime;
            if (patrolTimer >= 2.0f) {
                aiState = AIState::PATROL;
                generatePatrolTarget();
                patrolTimer = 0.0f;
            }
            
            // Check for player detection
            if (distanceToPlayer <= detectionRange) {
                aiState = AIState::CHASE;
            }
            break;
            
        case AIState::PATROL:
            // Move towards patrol target
            moveTowards(patrolTarget, deltaTime);
            
            // Check if reached patrol target
            if (distanceTo(patrolTarget) < 10.0f) {
                aiState = AIState::IDLE;
                patrolTimer = 0.0f;
            }
            
            // Check for player detection
            if (distanceToPlayer <= detectionRange) {
                aiState = AIState::CHASE;
            }
            break;
            
        case AIState::CHASE:
            // Chase the player
            moveTowards(player.getPosition(), deltaTime);
            
            // Check if close enough to attack
            if (distanceToPlayer <= attackRange) {
                aiState = AIState::ATTACK;
            }
            
            // Lose interest if player gets too far away
            if (distanceToPlayer > detectionRange * 1.5f) {
                aiState = AIState::PATROL;
                generatePatrolTarget();
            }
            break;
            
        case AIState::ATTACK:
            // Stop moving and attack
            if (distanceToPlayer <= attackRange) {
                // Face the player but don't move
                sf::Vector2f direction = normalize(player.getPosition() - position);
                // Could add facing direction here for sprite rotation
                
            } else {
                // Player moved away, go back to chasing
                aiState = AIState::CHASE;
            }
            break;
            
        case AIState::DEAD:
            // Do nothing
            break;
    }
    
    // Reset color after visual effects
    if (sprite.getFillColor() != color && aiState != AIState::DEAD) {
        sprite.setFillColor(color);
    }
}

void Enemy::moveTowards(sf::Vector2f target, float deltaTime) {
    sf::Vector2f direction = normalize(target - position);
    velocity = direction * speed;
}

bool Enemy::isColliding(const sf::FloatRect& other) const {
    return sprite.getGlobalBounds().intersects(other);
}

int Enemy::getExperienceReward() const {
    switch (type) {
        case EnemyType::GOBLIN: return 25;
        case EnemyType::ORC: return 50;
        case EnemyType::SKELETON: return 35;
        default: return 20;
    }
}

float Enemy::distanceTo(sf::Vector2f target) const {
    sf::Vector2f diff = target - position;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

sf::Vector2f Enemy::normalize(sf::Vector2f vector) const {
    float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    if (length == 0) return sf::Vector2f(0, 0);
    return sf::Vector2f(vector.x / length, vector.y / length);
}

void Enemy::generatePatrolTarget() {
    // Generate a random patrol target within a reasonable range
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    patrolTarget = position + sf::Vector2f(dist(gen), dist(gen));
}