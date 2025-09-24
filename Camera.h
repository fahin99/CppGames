#pragma once
#include <SFML/Graphics.hpp>

class Camera {
private:
    sf::View view;
    sf::Vector2f targetPosition;
    float smoothness;
    
public:
    Camera(float width, float height);
    
    void update(sf::Vector2f playerPosition, float deltaTime);
    void setTarget(sf::Vector2f target);
    sf::View getView() const { return view; }
    void setView(sf::RenderWindow& window);
    void shake(float intensity, float duration);
    
private:
    // Screen shake
    float shakeIntensity;
    float shakeDuration;
    float shakeTimer;
    sf::Vector2f shakeOffset;
};