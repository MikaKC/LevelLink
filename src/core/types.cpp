#include "types.h"

namespace ll {

level_link_type str_to_level_link_type(const std::string& str) {
    if(str.compare("editor") == 0) return level_link_type::editor;
    else if(str.compare("online") == 0) return level_link_type::online;
    
    return level_link_type::undefined;
}

std::string level_link_type_to_str(level_link_type type) {
    switch(type) {
        case level_link_type::editor: return "editor";
        case level_link_type::online: return "online";
    }

    return "undefined";
}

}