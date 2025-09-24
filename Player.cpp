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
    // Handle input
    handleInput();
    
    // Update attack cooldown
    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
        if (attackCooldown <= 0) {
            isAttacking = false;
        }
    }
    
    // Apply movement
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
    // Draw attack indicator
    if (isAttacking) {
        sf::CircleShape attackCircle(30);
        attackCircle.setFillColor(sf::Color(255, 255, 0, 100));
        attackCircle.setPosition(position.x - 30, position.y - 30);
        window.draw(attackCircle);
    }
    
    window.draw(sprite);
}

void Player::handleInput() {
    // Movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        velocity.y = -speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        velocity.y = speed;
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
    int actualDamage = std::max(1, damage - stats.defense);
    stats.health = std::max(0, stats.health - actualDamage);
    
    // Visual feedback
    sprite.setFillColor(sf::Color::Red);
    
    std::cout << "Player took " << actualDamage << " damage! Health: " << stats.health << "/" << stats.maxHealth << std::endl;
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