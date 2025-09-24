#pragma once
#include <string>
#include <vector>

struct UserData {
    std::string username;
    std::string password;
    int highScore;
    int totalGamesPlayed;
    std::string lastPlayed;
    
    UserData() : highScore(0), totalGamesPlayed(0), lastPlayed("") {}
};

class UserManager {
private:
    std::vector<UserData> users;
    std::string currentUser;
    std::string dataFile;
    
public:
    UserManager();
    
    bool loadUsers();
    bool saveUsers();
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool userExists(const std::string& username);
    void updateUserStats(int score);
    UserData getCurrentUserData();
    std::string getCurrentUsername() const { return currentUser; }
};