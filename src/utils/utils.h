#ifndef UTILS_H
#define UTILS_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

std::vector<GJGameLevel*> search_levels(const std::vector<GJGameLevel*>& levels, const std::string& match);

}

#endif