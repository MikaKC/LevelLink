#include "button_sprite_utils.h"

namespace ll {

const char* get_btn_texture_for_base(button_sprite_base base) {
    switch(base) {
        case button_sprite_base::green:      return "GJ_button_01.png";
        case button_sprite_base::blue:       return "GJ_button_02.png";
        case button_sprite_base::pink:       return "GJ_button_03.png";
        case button_sprite_base::light_grey: return "GJ_button_04.png";
        case button_sprite_base::dark_grey:  return "GJ_button_05.png";
        case button_sprite_base::red:        return "GJ_button_06.png";
    }
    
    return NULL;
}

}