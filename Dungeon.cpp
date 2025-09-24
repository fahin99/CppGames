#include "Dungeon.h"
#include <algorithm>
#include <iostream>

Dungeon::Dungeon(int w, int h) 
    : width(w), height(h), rng(std::random_device{}()) {
    
    // Initialize the tile grid
    tiles.resize(height);
    for (int y = 0; y < height; y++) {
        tiles[y].reserve(width);
        for (int x = 0; x < width; x++) {
            tiles[y].emplace_back(TileType::WALL, x * TILE_SIZE, y * TILE_SIZE, x, y);
        }
    }
}

void Dungeon::generate() {
    // Clear previous generation
    rooms.clear();
    
    // Fill with walls initially
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            setTile(x, y, TileType::WALL);
        }
    }
    
    // Generate rooms
    generateRooms();
    
    // Connect rooms with corridors
    generateCorridors();
    
    // Place treasures
    placeTreasures();
    
    // Set player spawn point (first room center)
    if (!rooms.empty()) {
        playerSpawn = sf::Vector2i(rooms[0].center.x * TILE_SIZE + TILE_SIZE/2, 
                                  rooms[0].center.y * TILE_SIZE + TILE_SIZE/2);
        setTile(rooms[0].center.x, rooms[0].center.y, TileType::SPAWN);
    }
}

void Dungeon::generateRooms() {
    std::uniform_int_distribution<int> roomCountDist(5, 10);
    std::uniform_int_distribution<int> roomSizeDist(4, 10);
    std::uniform_int_distribution<int> xPosDist(1, width - 12);
    std::uniform_int_distribution<int> yPosDist(1, height - 12);
    
    int numRooms = roomCountDist(rng);
    int attempts = 0;
    const int maxAttempts = 100;
    
    while (rooms.size() < numRooms && attempts < maxAttempts) {
        int roomWidth = roomSizeDist(rng);
        int roomHeight = roomSizeDist(rng);
        int x = xPosDist(rng);
        int y = yPosDist(rng);
        
        // Ensure room fits in bounds
        if (x + roomWidth >= width - 1 || y + roomHeight >= height - 1) {
            attempts++;
            continue;
        }
        
        Room newRoom(x, y, roomWidth, roomHeight);
        
        // Check if room intersects with existing rooms
        bool intersects = false;
        for (const auto& existingRoom : rooms) {
            if (newRoom.intersects(existingRoom)) {
                intersects = true;
                break;
            }
        }
        
        if (!intersects) {
            rooms.push_back(newRoom);
            
            // Carve out the room (make floor tiles)
            for (int ry = newRoom.y; ry < newRoom.y + newRoom.height; ry++) {
                for (int rx = newRoom.x; rx < newRoom.x + newRoom.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
        }
        
        attempts++;
    }
    
    std::cout << "Generated " << rooms.size() << " rooms" << std::endl;
}

void Dungeon::generateCorridors() {
    // Sort rooms by center position to create more logical connections
    std::sort(rooms.begin(), rooms.end(), [](const Room& a, const Room& b) {
        return a.center.x < b.center.x;
    });
    
    // Connect each room to the next one
    for (size_t i = 0; i < rooms.size() - 1; i++) {
        sf::Vector2i start = rooms[i].center;
        sf::Vector2i end = rooms[i + 1].center;
        
        // Create L-shaped corridor
        if (rng() % 2 == 0) {
            // Horizontal first, then vertical
            createHorizontalTunnel(start.x, end.x, start.y);
            createVerticalTunnel(start.y, end.y, end.x);
        } else {
            // Vertical first, then horizontal
            createVerticalTunnel(start.y, end.y, start.x);
            createHorizontalTunnel(start.x, end.x, end.y);
        }
    }
    
    // Add some extra connections for more interesting layouts
    std::uniform_int_distribution<int> extraConnections(0, 2);
    int numExtra = extraConnections(rng);
    
    for (int i = 0; i < numExtra && rooms.size() > 2; i++) {
        std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
        size_t room1 = roomDist(rng);
        size_t room2 = roomDist(rng);
        
        if (room1 != room2) {
            sf::Vector2i start = rooms[room1].center;
            sf::Vector2i end = rooms[room2].center;
            
            createHorizontalTunnel(start.x, end.x, start.y);
            createVerticalTunnel(start.y, end.y, end.x);
        }
    }
}

void Dungeon::createHorizontalTunnel(int x1, int x2, int y) {
    int startX = std::min(x1, x2);
    int endX = std::max(x1, x2);
    
    for (int x = startX; x <= endX; x++) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            setTile(x, y, TileType::FLOOR);
        }
    }
}

void Dungeon::createVerticalTunnel(int y1, int y2, int x) {
    int startY = std::min(y1, y2);
    int endY = std::max(y1, y2);
    
    for (int y = startY; y <= endY; y++) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            setTile(x, y, TileType::FLOOR);
        }
    }
}

void Dungeon::placeTreasures() {
    std::uniform_int_distribution<int> treasureChance(1, 10);
    
    for (const auto& room : rooms) {
        // 30% chance to place treasure in each room
        if (treasureChance(rng) <= 3) {
            // Place treasure in a random floor tile within the room
            std::uniform_int_distribution<int> xDist(room.x + 1, room.x + room.width - 2);
            std::uniform_int_distribution<int> yDist(room.y + 1, room.y + room.height - 2);
            
            int treasureX = xDist(rng);
            int treasureY = yDist(rng);
            
            setTile(treasureX, treasureY, TileType::TREASURE);
        }
    }
}

void Dungeon::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        tiles[y][x] = Tile(type, x * TILE_SIZE, y * TILE_SIZE, x, y);
    }
}

void Dungeon::render(sf::RenderWindow& window, const sf::View& view) {
    // Get the view bounds to only render visible tiles
    sf::FloatRect viewBounds;
    viewBounds.left = view.getCenter().x - view.getSize().x / 2;
    viewBounds.top = view.getCenter().y - view.getSize().y / 2;
    viewBounds.width = view.getSize().x;
    viewBounds.height = view.getSize().y;
    
    // Calculate tile range to render
    int startX = std::max(0, static_cast<int>(viewBounds.left / TILE_SIZE) - 1);
    int endX = std::min(width, static_cast<int>((viewBounds.left + viewBounds.width) / TILE_SIZE) + 2);
    int startY = std::max(0, static_cast<int>(viewBounds.top / TILE_SIZE) - 1);
    int endY = std::min(height, static_cast<int>((viewBounds.top + viewBounds.height) / TILE_SIZE) + 2);
    
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            window.draw(tiles[y][x].shape);
        }
    }
}

bool Dungeon::isWall(float x, float y) const {
    int gridX = static_cast<int>(x / TILE_SIZE);
    int gridY = static_cast<int>(y / TILE_SIZE);
    
    if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height) {
        return true; // Out of bounds is considered a wall
    }
    
    return tiles[gridY][gridX].type == TileType::WALL;
}

bool Dungeon::isValidPosition(float x, float y) const {
    return !isWall(x, y);
}

TileType Dungeon::getTileType(float x, float y) const {
    int gridX = static_cast<int>(x / TILE_SIZE);
    int gridY = static_cast<int>(y / TILE_SIZE);
    
    if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height) {
        return TileType::WALL;
    }
    
    return tiles[gridY][gridX].type;
}

sf::Vector2f Dungeon::getPlayerSpawn() const {
    return sf::Vector2f(playerSpawn.x, playerSpawn.y);
}

std::vector<sf::Vector2f> Dungeon::getEnemySpawns(int count) const {
    std::vector<sf::Vector2f> spawns;
    std::mt19937 localRng(std::random_device{}());
    std::uniform_int_distribution<int> roomDist(1, rooms.size() - 1); // Skip first room (player spawn)
    
    for (int i = 0; i < count && rooms.size() > 1; i++) {
        size_t roomIndex = roomDist(localRng);
        const Room& room = rooms[roomIndex];
        
        // Random position within the room
        std::uniform_int_distribution<int> xDist(room.x + 1, room.x + room.width - 2);
        std::uniform_int_distribution<int> yDist(room.y + 1, room.y + room.height - 2);
        
        float spawnX = xDist(localRng) * TILE_SIZE + TILE_SIZE / 2;
        float spawnY = yDist(localRng) * TILE_SIZE + TILE_SIZE / 2;
        
        spawns.emplace_back(spawnX, spawnY);
    }
    
    return spawns;
}

void Dungeon::setTileTypeAt(float x, float y, TileType type) {
    int gridX = static_cast<int>(x / TILE_SIZE);
    int gridY = static_cast<int>(y / TILE_SIZE);
    
    if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
        setTile(gridX, gridY, type);
    }
}