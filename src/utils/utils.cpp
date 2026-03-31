#include "utils.h"

namespace ll {

/* Search for levels with a matching string */
std::vector<GJGameLevel*> search_levels(const std::vector<GJGameLevel*>& levels, const std::string& match) {
    std::string lower_match = match;
    
    std::transform(
        lower_match.begin(), 
        lower_match.end(), 
        lower_match.begin(), 
        tolower
    );

    std::vector<std::pair<int, GJGameLevel*>> scored;

    for(auto* level : levels) {
        std::string lower_name = level->m_levelName;
        std::transform(
            lower_name.begin(), 
            lower_name.end(), 
            lower_name.begin(), 
            tolower
        );

        int score = 0;

        if(lower_name == lower_match) {
            score = 300;
        } else if(lower_name.find(lower_match) == 0) {
            score = 200;
        } else if(lower_name.find(lower_match) != std::string::npos) {
            score = 100;
        } else {
            for(char c : lower_match) {
                if(lower_name.find(c) != std::string::npos) score++;
            }
        }

        if (score > 0) scored.push_back({score, level});
    }

    std::sort(
        scored.begin(), 
        scored.end(), 
        [](const auto& a, const auto& b) {
            return a.first > b.first;
        }
    );

    std::vector<GJGameLevel*> results;
    for(const auto& entry : scored) {
        results.push_back(entry.second);
    }

    return results;
}

}