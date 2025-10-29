#pragma once 
#include <vector>
#include <string>

struct ScoreEntry {
    std::string initials;
    int score;
};

class HighScoresManager {
public:
    HighScoresManager(const std::string& filename, size_t maxEntries = 10);
    void load();
    void save();
    void addScore(const std::string& initials, int score);
    const std::vector<ScoreEntry>& getScores() const;
private:
    std::string filename;
    size_t maxEntries;
    std::vector<ScoreEntry> scores;
};