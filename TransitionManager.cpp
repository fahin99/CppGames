#include "TransitionManager.h"

TransitionManager::TransitionManager(int windowWidth, int windowHeight) 
    : fadeAlpha(0.0f)
    , fadeSpeed(300.0f) // Fade speed
    , isFading(false)
    , fadingIn(false)
    , targetState(GameState::WELCOME)
    , currentState(GameState::WELCOME) {
    
    fadeOverlay.setSize(sf::Vector2f(windowWidth, windowHeight));
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
    fadeOverlay.setPosition(0, 0);
}

void TransitionManager::startTransition(GameState from, GameState to) {
    if (isFading) return; // Already transitioning
    
    currentState = from;
    targetState = to;
    isFading = true;
    fadingIn = true; // Start by fading in (to black)
    fadeAlpha = 0.0f;
}

void TransitionManager::update(float deltaTime) {
    if (!isFading) return;
    
    if (fadingIn) {
        // Fade to black
        fadeAlpha += fadeSpeed * deltaTime;
        if (fadeAlpha >= 255.0f) {
            fadeAlpha = 255.0f;
            fadingIn = false; // Switch to fading out
            currentState = targetState; // Change state in the middle of transition
        }
    } else {
        // Fade from black
        fadeAlpha -= fadeSpeed * deltaTime;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            isFading = false; // Transition complete
        }
    }
    
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeAlpha)));
}

void TransitionManager::render(sf::RenderWindow& window) {
    if (isFading && fadeAlpha > 0) {
        window.draw(fadeOverlay);
    }
}

bool TransitionManager::shouldChangeState() const {
    return isFading && !fadingIn && fadeAlpha == 255.0f;
}