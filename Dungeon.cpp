#include "Dungeon.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

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
    std::uniform_int_distribution<int> roomCountDist(8, 15); // More rooms in compact space
    std::uniform_int_distribution<int> roomSizeDist(3, 8);   // Smaller room sizes
    std::uniform_int_distribution<int> xPosDist(2, width - 10);
    std::uniform_int_distribution<int> yPosDist(2, height - 10);
    std::uniform_int_distribution<int> roomTypeDist(1, 10);
    
    int numRooms = roomCountDist(rng);
    int attempts = 0;
    const int maxAttempts = 150;
    
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
        
        // Check if room intersects with existing rooms (allow closer placement)
        bool intersects = false;
        for (const auto& existingRoom : rooms) {
            // Reduced buffer for tighter room placement
            Room bufferedRoom(existingRoom.x - 1, existingRoom.y - 1, 
                            existingRoom.width + 2, existingRoom.height + 2);
            if (newRoom.intersects(bufferedRoom)) {
                intersects = true;
                break;
            }
        }
        
        if (!intersects) {
            rooms.push_back(newRoom);
            
            // Generate different room types for complexity
            int roomType = roomTypeDist(rng);
            
            if (roomType <= 6) {
                // Standard rectangular room
                for (int ry = newRoom.y; ry < newRoom.y + newRoom.height; ry++) {
                    for (int rx = newRoom.x; rx < newRoom.x + newRoom.width; rx++) {
                        setTile(rx, ry, TileType::FLOOR);
                    }
                }
            } else if (roomType <= 8) {
                // L-shaped room
                createLShapedRoom(newRoom);
            } else {
                // Cross-shaped room
                createCrossShapedRoom(newRoom);
            }
            
            // Add interior features for some rooms
            if (roomWidth >= 5 && roomHeight >= 5 && roomTypeDist(rng) <= 3) {
                addRoomFeatures(newRoom);
            }
        }
        
        attempts++;
    }
    
    std::cout << "Generated " << rooms.size() << " rooms with complex layouts" << std::endl;
}

void Dungeon::generateCorridors() {
    if (rooms.empty()) return;
    
    // Create a minimum spanning tree for room connections
    std::vector<bool> connected(rooms.size(), false);
    connected[0] = true; // Start with first room
    
    // Connect all rooms with minimum spanning tree approach
    for (size_t connectedCount = 1; connectedCount < rooms.size(); connectedCount++) {
        float minDistance = std::numeric_limits<float>::max();
        size_t nearestRoom = 0;
        size_t connectToRoom = 0;
        
        // Find closest unconnected room to any connected room
        for (size_t i = 0; i < rooms.size(); i++) {
            if (!connected[i]) {
                for (size_t j = 0; j < rooms.size(); j++) {
                    if (connected[j]) {
                        float distance = std::sqrt(std::pow(rooms[i].center.x - rooms[j].center.x, 2) +
                                                 std::pow(rooms[i].center.y - rooms[j].center.y, 2));
                        if (distance < minDistance) {
                            minDistance = distance;
                            nearestRoom = i;
                            connectToRoom = j;
                        }
                    }
                }
            }
        }
        
        // Connect the nearest room
        connected[nearestRoom] = true;
        sf::Vector2i start = rooms[connectToRoom].center;
        sf::Vector2i end = rooms[nearestRoom].center;
        
        // Create more interesting corridor patterns
        std::uniform_int_distribution<int> corridorType(1, 4);
        int type = corridorType(rng);
        
        if (type == 1) {
            // Standard L-shaped corridor
            if (rng() % 2 == 0) {
                createHorizontalTunnel(start.x, end.x, start.y);
                createVerticalTunnel(start.y, end.y, end.x);
            } else {
                createVerticalTunnel(start.y, end.y, start.x);
                createHorizontalTunnel(start.x, end.x, end.y);
            }
        } else if (type == 2) {
            // Winding corridor with middle point
            int midX = (start.x + end.x) / 2;
            int midY = (start.y + end.y) / 2;
            createHorizontalTunnel(start.x, midX, start.y);
            createVerticalTunnel(start.y, midY, midX);
            createHorizontalTunnel(midX, end.x, midY);
            createVerticalTunnel(midY, end.y, end.x);
        } else {
            // Direct path with some width variation
            createHorizontalTunnel(start.x, end.x, start.y);
            createVerticalTunnel(start.y, end.y, end.x);
            // Add parallel paths for wider corridors
            if (std::abs(start.y - end.y) > 3) {
                createHorizontalTunnel(start.x, end.x, start.y + 1);
            }
            if (std::abs(start.x - end.x) > 3) {
                createVerticalTunnel(start.y, end.y, end.x + 1);
            }
        }
    }
    
    // Add extra connections for loops and shortcuts (more complex navigation)
    std::uniform_int_distribution<int> extraConnections(2, 5); // More extra connections
    int numExtra = extraConnections(rng);
    
    for (int i = 0; i < numExtra && rooms.size() > 2; i++) {
        std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
        size_t room1 = roomDist(rng);
        size_t room2 = roomDist(rng);
        
        if (room1 != room2) {
            // Only create connection if rooms are reasonably close
            float distance = std::sqrt(std::pow(rooms[room1].center.x - rooms[room2].center.x, 2) +
                                     std::pow(rooms[room1].center.y - rooms[room2].center.y, 2));
            
            if (distance < width / 3) { // Only connect if relatively close
                sf::Vector2i start = rooms[room1].center;
                sf::Vector2i end = rooms[room2].center;
                
                // Create shortcut corridor
                createHorizontalTunnel(start.x, end.x, start.y);
                createVerticalTunnel(start.y, end.y, end.x);
            }
        }
    }
    
    // Add some dead-end corridors for exploration
    std::uniform_int_distribution<int> deadEndCount(1, 3);
    int numDeadEnds = deadEndCount(rng);
    
    for (int i = 0; i < numDeadEnds && !rooms.empty(); i++) {
        std::uniform_int_distribution<size_t> roomDist(0, rooms.size() - 1);
        size_t roomIndex = roomDist(rng);
        sf::Vector2i start = rooms[roomIndex].center;
        
        std::uniform_int_distribution<int> lengthDist(3, 8);
        std::uniform_int_distribution<int> directionDist(1, 4);
        int length = lengthDist(rng);
        int direction = directionDist(rng);
        
        switch (direction) {
            case 1: // North
                createVerticalTunnel(start.y, std::max(1, start.y - length), start.x);
                break;
            case 2: // South
                createVerticalTunnel(start.y, std::min(height - 2, start.y + length), start.x);
                break;
            case 3: // West
                createHorizontalTunnel(start.x, std::max(1, start.x - length), start.y);
                break;
            case 4: // East
                createHorizontalTunnel(start.x, std::min(width - 2, start.x + length), start.y);
                break;
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

void Dungeon::createLShapedRoom(const Room& room) {
    std::uniform_int_distribution<int> orientationDist(1, 4);
    int orientation = orientationDist(rng);
    
    int halfWidth = room.width / 2;
    int halfHeight = room.height / 2;
    
    switch (orientation) {
        case 1: // Top-left L
            for (int ry = room.y; ry < room.y + halfHeight; ry++) {
                for (int rx = room.x; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            for (int ry = room.y + halfHeight; ry < room.y + room.height; ry++) {
                for (int rx = room.x; rx < room.x + halfWidth; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            break;
        case 2: // Top-right L
            for (int ry = room.y; ry < room.y + halfHeight; ry++) {
                for (int rx = room.x; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            for (int ry = room.y + halfHeight; ry < room.y + room.height; ry++) {
                for (int rx = room.x + halfWidth; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            break;
        case 3: // Bottom-left L
            for (int ry = room.y + halfHeight; ry < room.y + room.height; ry++) {
                for (int rx = room.x; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            for (int ry = room.y; ry < room.y + halfHeight; ry++) {
                for (int rx = room.x; rx < room.x + halfWidth; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            break;
        case 4: // Bottom-right L
            for (int ry = room.y + halfHeight; ry < room.y + room.height; ry++) {
                for (int rx = room.x; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            for (int ry = room.y; ry < room.y + halfHeight; ry++) {
                for (int rx = room.x + halfWidth; rx < room.x + room.width; rx++) {
                    setTile(rx, ry, TileType::FLOOR);
                }
            }
            break;
    }
}

void Dungeon::createCrossShapedRoom(const Room& room) {
    int centerX = room.x + room.width / 2;
    int centerY = room.y + room.height / 2;
    int armWidth = std::max(1, room.width / 4);
    int armHeight = std::max(1, room.height / 4);
    
    // Horizontal arm
    for (int ry = centerY - armHeight; ry <= centerY + armHeight; ry++) {
        for (int rx = room.x; rx < room.x + room.width; rx++) {
            if (rx >= 0 && rx < width && ry >= 0 && ry < height) {
                setTile(rx, ry, TileType::FLOOR);
            }
        }
    }
    
    // Vertical arm
    for (int ry = room.y; ry < room.y + room.height; ry++) {
        for (int rx = centerX - armWidth; rx <= centerX + armWidth; rx++) {
            if (rx >= 0 && rx < width && ry >= 0 && ry < height) {
                setTile(rx, ry, TileType::FLOOR);
            }
        }
    }
}

void Dungeon::addRoomFeatures(const Room& room) {
    std::uniform_int_distribution<int> featureDist(1, 10);
    int feature = featureDist(rng);
    
    if (feature <= 4) {
        // Add pillars in corners or center
        int pillarX = room.x + room.width / 2;
        int pillarY = room.y + room.height / 2;
        setTile(pillarX, pillarY, TileType::WALL);
    } else if (feature <= 6) {
        // Create small alcoves
        if (room.width >= 6 && room.height >= 4) {
            int alcoveX = room.x + 1;
            int alcoveY = room.y + room.height / 2;
            setTile(alcoveX, alcoveY, TileType::WALL);
            setTile(alcoveX + 1, alcoveY, TileType::WALL);
        }
    }
    // Other features could include water, different floor types, etc.
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