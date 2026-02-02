#ifndef ARSCREW_HIGHSCORESMANAGER_H
#define ARSCREW_HIGHSCORESMANAGER_H
#include <vector>
#include <string>

struct ScoreEntry {
    std::string initials;
    float score;
};

class HighScoresManager {
public:
    HighScoresManager(const std::string& filename, size_t maxEntries = 10);
    void load();
    void save();
    void addScore(const std::string& initials, float score);
    const std::vector<ScoreEntry>& getScores() const;
    bool isHighScore(float score) const;
private:
    std::string filename;
    size_t maxEntries;
    std::vector<ScoreEntry> scores;
};
#endif // ARSCREW_HIGHSCORESMANAGER_H