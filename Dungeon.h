#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

enum class TileType {
    WALL,
    FLOOR,
    DOOR,
    TREASURE,
    SPAWN
};

struct Tile {
    TileType type;
    sf::RectangleShape shape;
    sf::Vector2i gridPos;
    
    Tile(TileType t, float x, float y, int gx, int gy) 
        : type(t), gridPos(gx, gy) {
        shape.setSize(sf::Vector2f(32.0f, 32.0f));
        shape.setPosition(x, y);
        
        switch(t) {
            case TileType::WALL:
                shape.setFillColor(sf::Color(100, 100, 100));
                break;
            case TileType::FLOOR:
                shape.setFillColor(sf::Color(200, 200, 200));
                break;
            case TileType::DOOR:
                shape.setFillColor(sf::Color(139, 69, 19));
                break;
            case TileType::TREASURE:
                shape.setFillColor(sf::Color::Yellow);
                break;
            case TileType::SPAWN:
                shape.setFillColor(sf::Color::Green);
                break;
        }
    }
};

struct Room {
    int x, y, width, height;
    sf::Vector2i center;
    
    Room(int _x, int _y, int _w, int _h) 
        : x(_x), y(_y), width(_w), height(_h) {
        center.x = x + width / 2;
        center.y = y + height / 2;
    }
    
    bool intersects(const Room& other) const {
        return (x < other.x + other.width && x + width > other.x &&
                y < other.y + other.height && y + height > other.y);
    }
};

class Dungeon {
private:
    std::vector<std::vector<Tile>> tiles;
    std::vector<Room> rooms;
    int width, height;
    sf::Vector2i playerSpawn;
    std::mt19937 rng;
    
    void generateRooms();
    void generateCorridors();
    void createHorizontalTunnel(int x1, int x2, int y);
    void createVerticalTunnel(int y1, int y2, int x);
    void createLShapedRoom(const Room& room);
    void createCrossShapedRoom(const Room& room);
    void addRoomFeatures(const Room& room);
    void placeTreasures();
    void setTile(int x, int y, TileType type);
    
public:
    Dungeon(int w, int h);
    
    void generate();
    void render(sf::RenderWindow& window, const sf::View& view);
    bool isWall(float x, float y) const;
    bool isValidPosition(float x, float y) const;
    TileType getTileType(float x, float y) const;
    sf::Vector2f getPlayerSpawn() const;
    std::vector<sf::Vector2f> getEnemySpawns(int count) const;
    void setTileTypeAt(float x, float y, TileType type);
    int getRoomCount() const { return rooms.size(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    static const int TILE_SIZE = 32;
};