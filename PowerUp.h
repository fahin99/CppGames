#pragma once
#include <SFML/Graphics.hpp>

enum class PowerUpType {
    HEALTH_POTION,
    DAMAGE_BOOST,
    SPEED_BOOST,
    ARMOR_BOOST
};

class PowerUp {
private:
    PowerUpType type;
    sf::Vector2f position;
    bool collected;
    sf::RectangleShape shape;
    float effectDuration;
    int effectValue;

public:
    PowerUp(PowerUpType t, float x, float y);
    
    void render(sf::RenderWindow& window);
    bool checkCollision(sf::Vector2f playerPos, float playerSize);
    void collect();
    
    bool isCollected() const { return collected; }
    PowerUpType getType() const { return type; }
    float getEffectDuration() const { return effectDuration; }
    int getEffectValue() const { return effectValue; }
    sf::Vector2f getPosition() const { return position; }
};