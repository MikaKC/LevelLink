#ifndef LINK_PARSER_H
#define LINK_PARSER_H

/* Level link format:
    * Simple string: "lsta_12345,0,23456,1;"
    * ;     <- end current link
    * id,0  <- editor link
    * id,1  <- online link  
*/

#include "types.h"

namespace ll::parser {

#define LL_ARGUMENT_SEPARATOR ','
#define LL_DEFINITION_SEPARATOR ';'
#define LL_FILE_START "lsta_"

std::vector<level_link_data> load_links(const std::string& link_data);
level_link_data load_link(const std::string& link);

std::string save_link(level_link_data link);
std::string save_links(const std::vector<level_link_data>& link_data);

}

#endif