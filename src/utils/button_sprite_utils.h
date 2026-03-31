#ifndef BUTTON_SPRITE_UTILS_H
#define BUTTON_SPRITE_UTILS_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

enum class button_sprite_base {
    green = 0,
    blue = 1,
    pink = 2,
    light_grey = 3,
    dark_grey = 4,
    red = 5
};

const char* get_btn_texture_for_base(button_sprite_base base);

}

#endif