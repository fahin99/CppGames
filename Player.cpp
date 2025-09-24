#include "Player.h"
#include <iostream>
#include <cmath>

const float Player::PLAYER_SIZE = 24.0f;
const float Player::PLAYER_SPEED = 200.0f;

Player::Player(float x, float y) 
    : position(x, y)
    , velocity(0, 0)
    , isAttacking(false)
    , attackCooldown(0.0f)
    , speed(PLAYER_SPEED)
    , animationTimer(0.0f)
    , currentFrame(0) {
    
    // Initialize sprite
    sprite.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    sprite.setPosition(position);
    color = sf::Color::Blue;
    sprite.setFillColor(color);
    
    // Center the origin
    sprite.setOrigin(PLAYER_SIZE / 2, PLAYER_SIZE / 2);
}

void Player::update(float deltaTime) {
    // Update power-ups
    updatePowerUps(deltaTime);
    
    // Handle input
    handleInput();
    
    // Update attack cooldown
    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
        if (attackCooldown <= 0) {
            isAttacking = false;
        }
    }
    
    // Apply movement with effective speed
    position += velocity * deltaTime;
    sprite.setPosition(position);
    
    // Update animation
    animationTimer += deltaTime;
    if (animationTimer >= 0.2f) { // Change frame every 0.2 seconds
        currentFrame = (currentFrame + 1) % 4;
        animationTimer = 0.0f;
        
        // Simple color animation when moving
        if (velocity.x != 0 || velocity.y != 0) {
            float intensity = 0.8f + 0.2f * std::sin(currentFrame * 1.57f); // 90 degrees
            sprite.setFillColor(sf::Color(
                static_cast<sf::Uint8>(color.r * intensity),
                static_cast<sf::Uint8>(color.g * intensity),
                static_cast<sf::Uint8>(color.b * intensity)
            ));
        } else {
            sprite.setFillColor(color);
        }
    }
    
    // Reset velocity for next frame
    velocity = sf::Vector2f(0, 0);
}

void Player::render(sf::RenderWindow& window) {
    // Draw attack indicator - much more visible
    if (isAttacking) {
        // Large pulsing circle
        sf::CircleShape attackCircle(60);
        attackCircle.setFillColor(sf::Color(255, 255, 0, 150));
        attackCircle.setPosition(position.x - 60, position.y - 60);
        attackCircle.setOutlineThickness(3);
        attackCircle.setOutlineColor(sf::Color::Red);
        window.draw(attackCircle);
        
        // Smaller inner circle
        sf::CircleShape innerCircle(30);
        innerCircle.setFillColor(sf::Color(255, 0, 0, 200));
        innerCircle.setPosition(position.x - 30, position.y - 30);
        window.draw(innerCircle);
    }
    
    window.draw(sprite);
}

void Player::handleInput() {
    float effectiveSpeed = getEffectiveSpeed();
    
    // Movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -effectiveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = effectiveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        velocity.y = -effectiveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        velocity.y = effectiveSpeed;
    }
    
    // Normalize diagonal movement
    if (velocity.x != 0 && velocity.y != 0) {
        float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        velocity.x = velocity.x / length * speed;
        velocity.y = velocity.y / length * speed;
    }
    
    // Attack input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && attackCooldown <= 0) {
        attack();
    }
}

void Player::move(float dx, float dy) {
    position.x += dx;
    position.y += dy;
    sprite.setPosition(position);
}

void Player::attack() {
    if (attackCooldown <= 0) {
        isAttacking = true;
        attackCooldown = 0.5f; // 0.5 second cooldown
        
        sprite.setFillColor(sf::Color::Yellow);
    }
}

void Player::takeDamage(int damage) {
    int effectiveArmor = getEffectiveArmor();
    int actualDamage = std::max(1, damage - effectiveArmor);
    stats.health = std::max(0, stats.health - actualDamage);
    
    // Visual feedback
    sprite.setFillColor(sf::Color::Red);
    
    std::cout << "Player took " << actualDamage << " damage (reduced by " << effectiveArmor << " armor)! Health: " << stats.health << "/" << stats.maxHealth << std::endl;
}

void Player::gainExperience(int exp) {
    stats.experience += exp;
    
    // Check for level up
    int requiredExp = stats.level * 100; // 100 exp per level
    if (stats.experience >= requiredExp) {
        levelUp();
    }
}

void Player::levelUp() {
    stats.level++;
    stats.experience = 0; // Reset experience for next level
    
    // Increase stats
    stats.maxHealth += 20;
    stats.health = stats.maxHealth; // Full heal on level up
    stats.attack += 5;
    stats.defense += 2;
    
    std::cout << "Level up! Now level " << stats.level << std::endl;
    
    // Visual feedback
    sprite.setFillColor(sf::Color::Cyan);
}

void Player::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    sprite.setPosition(position);
}

void Player::setPosition(sf::Vector2f pos) {
    position = pos;
    sprite.setPosition(position);
}

void Player::applyPowerUp(int powerUpType, int value, float duration) {
    switch (powerUpType) {
        case 0: // HEALTH_POTION
            heal(value);
            break;
        case 1: // DAMAGE_BOOST
            activePowerUps.damageBoost = value;
            activePowerUps.damageBoostTimer = duration;
            break;
        case 2: // SPEED_BOOST
            activePowerUps.speedBoost = value;
            activePowerUps.speedBoostTimer = duration;
            break;
        case 3: // ARMOR_BOOST
            activePowerUps.armorBoost = value;
            activePowerUps.armorBoostTimer = duration;
            break;
    }
}

void Player::updatePowerUps(float deltaTime) {
    if (activePowerUps.damageBoostTimer > 0) {
        activePowerUps.damageBoostTimer -= deltaTime;
        if (activePowerUps.damageBoostTimer <= 0) {
            activePowerUps.damageBoost = 0;
        }
    }
    
    if (activePowerUps.speedBoostTimer > 0) {
        activePowerUps.speedBoostTimer -= deltaTime;
        if (activePowerUps.speedBoostTimer <= 0) {
            activePowerUps.speedBoost = 0;
        }
    }
    
    if (activePowerUps.armorBoostTimer > 0) {
        activePowerUps.armorBoostTimer -= deltaTime;
        if (activePowerUps.armorBoostTimer <= 0) {
            activePowerUps.armorBoost = 0;
        }
    }
}

void Player::heal(int amount) {
    stats.health = std::min(stats.maxHealth, stats.health + amount);
    std::cout << "Player healed for " << amount << " HP! Health: " << stats.health << "/" << stats.maxHealth << std::endl;
}

int Player::getEffectiveAttack() const {
    return stats.attack + activePowerUps.damageBoost;
}

int Player::getEffectiveSpeed() const {
    return PLAYER_SPEED + (PLAYER_SPEED * activePowerUps.speedBoost / 100);
}

int Player::getEffectiveArmor() const {
    return stats.defense + activePowerUps.armorBoost;
}