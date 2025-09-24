#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"

class TransitionManager {
private:
    sf::RectangleShape fadeOverlay;
    float fadeAlpha;
    float fadeSpeed;
    bool isFading;
    bool fadingIn;
    GameState targetState;
    GameState currentState;
    
public:
    TransitionManager(int windowWidth, int windowHeight);
    
    void startTransition(GameState from, GameState to);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    bool isTransitioning() const { return isFading; }
    GameState getCurrentState() const { return currentState; }
    GameState getTargetState() const { return targetState; }
    bool shouldChangeState() const;
};