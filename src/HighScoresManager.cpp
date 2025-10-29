#include "../include/arscrew/HighScoresManager.h"
#include <fstream>
#include <algorithm>

HighScoresManager::HighScoresManager(const std::string& filename, size_t maxEntries)
    : filename(filename), maxEntries(maxEntries) {}

void HighScoresManager::load() {
    scores.clear();
    std::ifstream file(filename);
    std::string initials;
    int score;
    while (file >> initials >> score) {
        scores.push_back({initials, score});
    }
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score >b.score;
    });
    if (scores.size() > maxEntries) {
        scores.resize(maxEntries);
}
}

void HighScoresManager::save() {
    std::ofstream file(filename, std::ios::trunc);
    for (const auto& entry : scores) {
        file << entry.initials << " " << entry.score << "\n";
    }
}

void HighScoresManager::addScore(const std::string& initials, int score) {
    scores.push_back({initials, score});
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });
    if (scores.size() > maxEntries) 
        scores.resize(maxEntries);
    save();
}

const std::vector<ScoreEntry>& HighScoresManager::getScores() const {
    return scores;
}