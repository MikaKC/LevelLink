#include "link_parser.h"
#include "../utils/utils.h"

#include <Geode/Geode.hpp>

#include <sstream>

namespace ll::parser {

std::vector<level_link_data> load_links(const std::string& link_data) {
    std::vector<level_link_data> ret{};
    std::string link = link_data;

    size_t file_start_len = strlen(LL_FILE_START);
    /* Valid link data should always start by file start token, otherwise we exit early */
    bool is_valid_ldata = false;
    if(link.compare(0, file_start_len, LL_FILE_START) == 0) is_valid_ldata = true;
    
    if(!is_valid_ldata) {
        geode::log::error("Link data is invalid!");
        return ret;
    }

    link = link.erase(0, file_start_len);

    if(link.length() <= 1) {
        geode::log::debug("Link is too short, returning null!");
        return ret;
    }

    std::vector<std::string> links = ll::utils::split_string_by_delimiter(link, LL_DEFINITION_SEPARATOR);
    
    level_link_data current_link;
    for(std::string& l : links) {
        current_link = ll::parser::load_link(l);
        ret.push_back(current_link);
    }

    return ret;
}

level_link_data load_link(const std::string& link) {
    std::vector<std::string> components = ll::utils::split_string_by_delimiter(link, LL_ARGUMENT_SEPARATOR);
    
    level_link_data invalid = {
        .level_id = -1,
        .level_type = level_link_type::undefined,
        .link_id = -1,
        .link_type = level_link_type::undefined,
    };
    
    if(components.size() < 4) {
        geode::log::error("Link object is incomplete!");
        return invalid;
    }

    auto level_id_r = geode::utils::numFromString<int>(components[0]);
    if(!level_id_r.isOk()) {
        geode::log::error("Failed to parse level id argument!");
        return invalid;
    }

    int level_id = level_id_r.unwrap();

    auto level_type_r = geode::utils::numFromString<int>(components[1]);
    if(!level_type_r.isOk()) {
        geode::log::error("Failed to parse level type argument!");
        return invalid;
    }
    
    level_link_type level_type = (level_link_type)(level_type_r.unwrap());

    auto link_id_r = geode::utils::numFromString<int>(components[2]);
    if(!link_id_r.isOk()) {
        geode::log::error("Failed to parse link id argument!");
        return invalid;
    }
    
    int link_id = link_id_r.unwrap();

    auto link_type_r = geode::utils::numFromString<int>(components[3]);
    if(!link_type_r.isOk()) {
        geode::log::error("Failed to parse link type argument!");
        return invalid;
    }

    level_link_type link_type = (level_link_type)(link_type_r.unwrap());

    return {
        .level_id = level_id,
        .level_type = level_type,
        .link_id = link_id,
        .link_type = link_type
    };
}

std::string save_link(level_link_data link) {
    std::string level_id = std::to_string(link.level_id);
    std::string level_type = std::to_string((int)link.level_type);
    
    std::string link_id = std::to_string(link.link_id);
    std::string link_type = std::to_string((int)link.link_type);

    auto add_arg = [](std::stringstream& o, const std::string& v, bool end_def = false) -> void {
        const char delimitor = (!end_def) ? LL_ARGUMENT_SEPARATOR : LL_DEFINITION_SEPARATOR;
        o << v;
        o << delimitor;
    };

    std::stringstream out{};
    add_arg(out, level_id);
    add_arg(out, level_type);
    add_arg(out, link_id);
    add_arg(out, link_type, true);
    
    return out.str();
}

std::string save_links(const std::vector<level_link_data>& link_data) {
    std::stringstream out{};
    out << LL_FILE_START;

    for(const level_link_data& data : link_data) {
        std::string def = ll::parser::save_link(data);
        out << def;
    }

    return out.str();
}

}