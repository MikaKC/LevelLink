#include "linking_manager.h"

#include "link_parser.h"

#define MOD Mod::get()

namespace ll {

linking_manager::linking_manager() : m_data({}) {}

void linking_manager::create_link(level_link_data data) {
    if(this->m_data.empty()) {
        this->load_links();
    }

    this->m_data.push_back(data);
    this->save_links();
}

void linking_manager::break_link(int level_id) {
    if(this->m_data.empty()) {
        this->load_links();
    }

    std::erase_if(this->m_data, [&](const auto& a){
        return (a.level_id == level_id) || (a.link_id == level_id);
    });
    this->save_links();
}

std::optional<level_link_data> linking_manager::get_link(int level_id) {
    if(this->m_data.empty()) {
        this->load_links();
    }

    const auto& it = std::find_if(this->m_data.begin(), this->m_data.end(), [&](const level_link_data& a) -> bool {
        return a.level_id == level_id || a.link_id == level_id;
    });
    
    if(it == this->m_data.end()) {
        return std::optional<level_link_data>();
    }

    const level_link_data& data = *it;
    return std::optional<level_link_data>(data);
}

const std::vector<level_link_data>& linking_manager::get_all_links() {
    return this->m_data;
}

void linking_manager::set_current_data(const level_link_data& data) {
    this->m_current_data = data;
}

void linking_manager::clear_current_data() {
    this->m_current_data = level_link_data {
        .level_id = -1,
        .level_type = level_link_type::undefined,
        .link_id = -1,
        .link_type = level_link_type::undefined
    };
}

level_link_data& linking_manager::get_current_data() {
    return this->m_current_data;
}

void linking_manager::load_links() {
    std::string linked_lvls_str = MOD->getSavedValue<std::string>("level_links", LL_FILE_START);
    this->m_data = ll::parser::load_links(linked_lvls_str);
}

void linking_manager::save_links() {
    std::string save = ll::parser::save_links(this->m_data);

    MOD->setSavedValue("level_links", save);
}

linking_manager& linking_manager::get() {
    static linking_manager instance;
    return instance;
}

}