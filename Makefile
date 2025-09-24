# Makefile for Dungeon Crawler

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SOURCES = main.cpp Game.cpp Player.cpp Enemy.cpp Dungeon.cpp Camera.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = dungeon_crawler

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(SFML_FLAGS)

# Build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install SFML on Ubuntu/Debian
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install libsfml-dev

# Install SFML on macOS
install-deps-macos:
	brew install sfml

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps-ubuntu install-deps-macos run