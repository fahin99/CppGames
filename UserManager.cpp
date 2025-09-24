#include "UserManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

UserManager::UserManager() : dataFile("users.json") {
    loadUsers();
}

bool UserManager::loadUsers() {
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        std::cout << "No existing user data found. Starting fresh." << std::endl;
        return true; // Not an error, just no existing data
    }
    
    users.clear();
    std::string line;
    
    // Simple JSON-like parsing (basic implementation)
    while (std::getline(file, line)) {
        if (line.find("\"username\":") != std::string::npos) {
            UserData user;
            
            // Extract username
            size_t start = line.find("\"username\":\"") + 12;
            size_t end = line.find("\"", start);
            user.username = line.substr(start, end - start);
            
            // Extract password
            std::getline(file, line);
            start = line.find("\"password\":\"") + 12;
            end = line.find("\"", start);
            user.password = line.substr(start, end - start);
            
            // Extract high score
            std::getline(file, line);
            start = line.find("\"highScore\":") + 12;
            end = line.find(",", start);
            if (end == std::string::npos) end = line.find("}", start);
            user.highScore = std::stoi(line.substr(start, end - start));
            
            // Extract total games
            std::getline(file, line);
            start = line.find("\"totalGamesPlayed\":") + 19;
            end = line.find(",", start);
            if (end == std::string::npos) end = line.find("}", start);
            user.totalGamesPlayed = std::stoi(line.substr(start, end - start));
            
            // Extract last played
            std::getline(file, line);
            start = line.find("\"lastPlayed\":\"") + 14;
            end = line.find("\"", start);
            user.lastPlayed = line.substr(start, end - start);
            
            users.push_back(user);
        }
    }
    
    file.close();
    std::cout << "Loaded " << users.size() << " users from file." << std::endl;
    return true;
}

bool UserManager::saveUsers() {
    std::ofstream file(dataFile);
    if (!file.is_open()) {
        std::cerr << "Failed to save user data!" << std::endl;
        return false;
    }
    
    file << "[\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const UserData& user = users[i];
        file << "  {\n";
        file << "    \"username\":\"" << user.username << "\",\n";
        file << "    \"password\":\"" << user.password << "\",\n";
        file << "    \"highScore\":" << user.highScore << ",\n";
        file << "    \"totalGamesPlayed\":" << user.totalGamesPlayed << ",\n";
        file << "    \"lastPlayed\":\"" << user.lastPlayed << "\"\n";
        file << "  }";
        if (i < users.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    
    file.close();
    return true;
}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) {
        return false;
    }
    
    UserData newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.highScore = 0;
    newUser.totalGamesPlayed = 0;
    newUser.lastPlayed = "Never";
    
    users.push_back(newUser);
    saveUsers();
    
    std::cout << "User " << username << " registered successfully!" << std::endl;
    return true;
}

bool UserManager::loginUser(const std::string& username, const std::string& password) {
    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            currentUser = username;
            std::cout << "User " << username << " logged in successfully!" << std::endl;
            return true;
        }
    }
    return false;
}

bool UserManager::userExists(const std::string& username) {
    return std::find_if(users.begin(), users.end(),
        [&username](const UserData& user) {
            return user.username == username;
        }) != users.end();
}

void UserManager::updateUserStats(int score) {
    for (auto& user : users) {
        if (user.username == currentUser) {
            user.totalGamesPlayed++;
            if (score > user.highScore) {
                user.highScore = score;
            }
            user.lastPlayed = "Today"; // Simplified for now
            break;
        }
    }
    saveUsers();
}

UserData UserManager::getCurrentUserData() {
    for (const auto& user : users) {
        if (user.username == currentUser) {
            return user;
        }
    }
    return UserData(); // Return empty if not found
}