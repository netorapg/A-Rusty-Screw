#include "../include/arscrew/HighScoresManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

HighScoresManager::HighScoresManager(const std::string& filename, size_t maxEntries)
    : filename(filename), maxEntries(maxEntries) {}

void HighScoresManager::load() {
    std::cout << "=== DEBUG HighScoresManager::load() ===" << std::endl;
    std::cout << "Tentando carregar arquivo: " << filename << std::endl;
    
    scores.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Erro ao abrir arquivo de highscores!" << std::endl;
        return;
    }
    
    std::string initials;
    float score;
    int count = 0;
    while (file >> initials >> score) {
        std::cout << "Carregando score " << count << ": " << initials << " - " << score << std::endl;
        scores.push_back({initials, score});
        count++;
    }
    
    std::cout << "Total de scores carregados: " << scores.size() << std::endl;
    
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
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

void HighScoresManager::addScore(const std::string& initials, float score) {
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

bool HighScoresManager::isHighScore(float score) const {
    if (scores.size() < maxEntries) {
        return true; // Sempre é highscore se não temos 10 scores ainda
    }
    
    // Verifica se o score é maior que o menor score na lista
    return score > scores.back().score;
}