#include "PowerUp.h"
#include <cmath>

PowerUp::PowerUp(PowerUpType t, float x, float y) 
    : type(t), position(x, y), collected(false) {
    
    shape.setSize(sf::Vector2f(20, 20));
    shape.setPosition(x - 10, y - 10);
    
    // Set properties based on type
    switch (type) {
        case PowerUpType::HEALTH_POTION:
            shape.setFillColor(sf::Color::Red);
            effectValue = 30; // Heal 30 HP
            effectDuration = 0.0f; // Instant effect
            break;
            
        case PowerUpType::DAMAGE_BOOST:
            shape.setFillColor(sf::Color::Yellow);
            effectValue = 15; // +15 damage
            effectDuration = 10.0f; // 10 seconds
            break;
            
        case PowerUpType::SPEED_BOOST:
            shape.setFillColor(sf::Color::Green);
            effectValue = 50; // +50% speed
            effectDuration = 8.0f; // 8 seconds
            break;
            
        case PowerUpType::ARMOR_BOOST:
            shape.setFillColor(sf::Color::Blue);
            effectValue = 5; // +5 armor (damage reduction)
            effectDuration = 12.0f; // 12 seconds
            break;
    }
    
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);
}

void PowerUp::render(sf::RenderWindow& window) {
    if (!collected) {
        // Add pulsing effect
        static float pulseTimer = 0.0f;
        pulseTimer += 0.02f;
        float pulse = 0.8f + 0.2f * std::sin(pulseTimer * 4.0f);
        
        sf::Color currentColor = shape.getFillColor();
        currentColor.a = (sf::Uint8)(255 * pulse);
        shape.setFillColor(currentColor);
        
        window.draw(shape);
        
        // Draw power-up symbol in center
        sf::RectangleShape symbol(sf::Vector2f(8, 8));
        symbol.setPosition(position.x - 4, position.y - 4);
        symbol.setFillColor(sf::Color::White);
        
        switch (type) {
            case PowerUpType::HEALTH_POTION:
                // Draw cross symbol
                symbol.setSize(sf::Vector2f(6, 2));
                symbol.setPosition(position.x - 3, position.y - 1);
                window.draw(symbol);
                symbol.setSize(sf::Vector2f(2, 6));
                symbol.setPosition(position.x - 1, position.y - 3);
                window.draw(symbol);
                break;
                
            case PowerUpType::DAMAGE_BOOST:
                // Draw sword symbol (vertical line)
                symbol.setSize(sf::Vector2f(2, 8));
                symbol.setPosition(position.x - 1, position.y - 4);
                window.draw(symbol);
                break;
                
            case PowerUpType::SPEED_BOOST:
                // Draw arrow symbol
                symbol.setSize(sf::Vector2f(6, 2));
                symbol.setPosition(position.x - 3, position.y - 1);
                window.draw(symbol);
                symbol.setSize(sf::Vector2f(2, 2));
                symbol.setPosition(position.x + 2, position.y - 2);
                window.draw(symbol);
                symbol.setPosition(position.x + 2, position.y);
                window.draw(symbol);
                break;
                
            case PowerUpType::ARMOR_BOOST:
                // Draw shield symbol
                symbol.setSize(sf::Vector2f(6, 6));
                symbol.setPosition(position.x - 3, position.y - 3);
                window.draw(symbol);
                break;
        }
    }
}

bool PowerUp::checkCollision(sf::Vector2f playerPos, float playerSize) {
    if (collected) return false;
    
    float distance = std::sqrt(std::pow(position.x - playerPos.x, 2) + 
                              std::pow(position.y - playerPos.y, 2));
    return distance <= (playerSize + 10);
}

void PowerUp::collect() {
    collected = true;
}