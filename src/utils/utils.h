#ifndef UTILS_H
#define UTILS_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll::utils {

std::vector<GJGameLevel*> search_levels(const std::vector<GJGameLevel*>& levels, const std::string& match);
std::vector<std::string> split_string_by_delimiter(const std::string& s, char delim);

/* Useful to abstract EditorIDs */
int get_real_level_id(GJGameLevel* level);

}

#endif