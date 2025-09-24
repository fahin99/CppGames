# Dungeon Crawler - SFML Rogue-like Game

A fully functional side-scrolling dungeon crawler rogue-like game built with C++ and SFML.

## Features

- **Procedural Dungeon Generation**: Each playthrough features a randomly generated dungeon with rooms, corridors, and treasures
- **Player Character**: 
  - Smooth movement with WASD/Arrow keys
  - Attack system with Space key
  - Experience points and leveling system
  - Health and stats progression
- **Enemy AI System**: 
  - Three enemy types: Goblins, Orcs, and Skeletons
  - Different AI states: Patrol, Chase, Attack
  - Unique stats and behaviors for each enemy type
- **Combat System**: 
  - Real-time combat with attack cooldowns
  - Damage calculation with defense stats
  - Experience rewards for defeating enemies
- **Side-scrolling Camera**: 
  - Smooth camera following the player
  - Screen shake effects for combat feedback
- **Game States**: 
  - Main menu, playing, paused, and game over states
  - Score tracking and progression
- **Treasure Collection**: 
  - Find treasures throughout the dungeon
  - Bonus experience and score rewards

## Requirements

- C++17 compatible compiler (GCC, Clang, or MSVC)
- SFML 2.5+ library
- CMake (optional, for easier building)

## Building the Game

### Windows (Visual Studio)
1. Install SFML library
2. Create a new C++ project
3. Add all source files (.cpp) and headers (.h)
4. Link SFML libraries: sfml-graphics, sfml-window, sfml-system
5. Compile and run

### Windows (Code::Blocks or Dev-C++)
1. Install SFML
2. Create new project and add all files
3. Configure linker settings for SFML
4. Build and run

### Linux/macOS
```bash
# Install SFML (Ubuntu/Debian)
sudo apt-get install libsfml-dev

# Install SFML (macOS with Homebrew)
brew install sfml

# Compile
g++ -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler

# Run
./dungeon_crawler
```

## Controls

- **Movement**: WASD or Arrow Keys
- **Attack**: Space
- **Pause**: Escape (during gameplay)
- **Start Game**: Space (from main menu)
- **Restart**: R (from game over screen)

## Game Mechanics

### Player Progression
- Start at Level 1 with basic stats
- Gain experience by defeating enemies and collecting treasures
- Level up to increase health, attack, and defense
- Full heal on level up

### Enemy Types
- **Goblin**: Fast, weak, low health
- **Orc**: Strong, slow, high health and defense
- **Skeleton**: Medium stats, fast attack speed

### Dungeon Layout
- Randomly generated rooms connected by corridors
- Treasures spawn in some rooms
- Player spawns in the first generated room
- Enemies spawn throughout other rooms

## Code Structure

- `main.cpp`: Entry point
- `Game.h/cpp`: Main game class with game loop and state management
- `Player.h/cpp`: Player character with movement, combat, and progression
- `Enemy.h/cpp`: Enemy AI and behavior system
- `Dungeon.h/cpp`: Procedural dungeon generation
- `Camera.h/cpp`: Side-scrolling camera with smooth following and screen shake
- `GameState.h`: Game state enumeration

## Future Enhancements

- Sound effects and background music
- More enemy types and boss battles
- Power-ups and equipment system
- Multiple dungeon levels
- Save/load functionality
- Particle effects
- Improved graphics with sprites instead of colored rectangles

## License

This project is provided as-is for educational purposes. Feel free to modify and expand upon it!