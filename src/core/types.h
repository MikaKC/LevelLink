#ifndef TYPES_H
#define TYPES_H

#include <string>

namespace ll {

enum class level_link_type {
    undefined = -1,
    editor = 0,
    online = 1
};

level_link_type str_to_level_link_type(const std::string& str);
std::string level_link_type_to_str(level_link_type type);

typedef struct level_link_data {
    int level_id;
    level_link_type level_type;

    int link_id;
    level_link_type link_type;
} level_link_data;

}

#endif