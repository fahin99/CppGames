#include "Camera.h"
#include <cmath>
#include <random>

Camera::Camera(float width, float height) 
    : smoothness(5.0f)
    , shakeIntensity(0.0f)
    , shakeDuration(0.0f)
    , shakeTimer(0.0f)
    , shakeOffset(0, 0) {
    
    view.setSize(width, height);
    view.setCenter(width / 2, height / 2);
    targetPosition = view.getCenter();
}

void Camera::update(sf::Vector2f playerPosition, float deltaTime) {
    // Update target position to player position
    targetPosition = playerPosition;
    
    // Smooth camera movement
    sf::Vector2f currentCenter = view.getCenter();
    sf::Vector2f direction = targetPosition - currentCenter;
    
    // Apply smoothing
    sf::Vector2f newCenter = currentCenter + direction * smoothness * deltaTime;
    
    // Update shake effect
    if (shakeTimer > 0) {
        shakeTimer -= deltaTime;
        
        // Generate random shake offset
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> shakeDist(-shakeIntensity, shakeIntensity);
        
        shakeOffset.x = shakeDist(gen);
        shakeOffset.y = shakeDist(gen);
        
        // Reduce shake intensity over time
        float shakeProgress = shakeTimer / shakeDuration;
        shakeOffset.x *= shakeProgress;
        shakeOffset.y *= shakeProgress;
    } else {
        shakeOffset = sf::Vector2f(0, 0);
    }
    
    // Apply final position with shake
    view.setCenter(newCenter + shakeOffset);
}

void Camera::setTarget(sf::Vector2f target) {
    targetPosition = target;
}

void Camera::setView(sf::RenderWindow& window) {
    window.setView(view);
}

void Camera::shake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeDuration = duration;
    shakeTimer = duration;
}