#include "utils.h"
#include <cvolton.level-id-api/include/EditorIDs.hpp>

namespace ll::utils {

/* Simple level query */
std::vector<GJGameLevel*> search_levels(const std::vector<GJGameLevel*>& levels, const std::string& match) {
    std::string lower_match = match;
    
    std::transform(
        lower_match.begin(), 
        lower_match.end(), 
        lower_match.begin(), 
        tolower
    );

    std::vector<std::pair<uint16_t, GJGameLevel*>> scored;

    for(auto* level : levels) {
        if(!level) continue;

        std::string lower_name = level->m_levelName;
        std::transform(
            lower_name.begin(), 
            lower_name.end(), 
            lower_name.begin(), 
            tolower
        );

        uint16_t score = 0;
        if(lower_name == lower_match) { /* Exact match */
            score = 300;
        } else if(lower_name.find(lower_match) == 0) { /* Beginning match */
            score = 200;
        } else if(lower_name.find(lower_match) != std::string::npos) { /* Contains match */
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

std::vector<std::string> split_string_by_delimiter(const std::string& s, char delim) {
    std::vector<std::string> ret{};
    size_t start = 0;
    size_t end = 0;

    while((end=s.find(delim, start)) != std::string::npos) {
        if(end != start) ret.push_back(s.substr(start, end - start));
        start = end+1;
    }
    
    if (start < s.size()) ret.push_back(s.substr(start));

    return ret;
}

int get_real_level_id(GJGameLevel* level) {
    if(!level) return -1;
    GJLevelType level_type = level->m_levelType;

    if(level_type == GJLevelType::Editor) return EditorIDs::getID(level);
    return level->m_levelID;
}


}