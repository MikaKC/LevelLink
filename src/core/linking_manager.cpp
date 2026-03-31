#include "linking_manager.h"

#define MOD Mod::get()

namespace ll {

linking_manager::linking_manager() : m_link_data(nh::json::array()), m_is_info_layer(false), m_cached_data({}) {}

void linking_manager::create_link(level_link_data data) {
    if(this->m_link_data.empty()) {
        this->load_links();
    }

    if(!this->m_link_data.is_array()) {
        this->m_link_data = nh::json::array();
    }

    nh::json link_object = {
        { "level_id", data.level_id },
        { "level_type", ll::level_link_type_to_str(data.level_type) },
        { "link_id", data.link_id },
        { "link_type", ll::level_link_type_to_str(data.link_type) }
    };

    this->m_link_data.push_back(link_object);
    if(!this->m_cached_data.empty()) this->m_cached_data.push_back(data);

    this->save_links();
}

void linking_manager::break_link(int level_id) {
    if(this->m_link_data.empty()) {
        this->load_links();
    }

    for(auto it = this->m_link_data.begin(); it != this->m_link_data.end(); ++it) {
        auto& link = *it;
        
        bool is_level = link["level_id"] == level_id;
        bool is_linked = link["link_id"] == level_id; 
        if(is_level || is_linked) {
            this->m_link_data.erase(it);
            
            if(!this->m_cached_data.empty()) {
                std::erase_if(this->m_cached_data, [&](const auto& a){
                    bool cond = false;
                    if(is_level) cond = (a.level_id == level_id);
                    else if(is_linked) cond = (a.link_id == level_id);
                    return cond;
                });
            }

            this->save_links();
            return;
        }
    }
}

std::optional<level_link_data> linking_manager::get_link(int level_id) {
    if(this->m_link_data.empty()) {
        this->load_links();
    }

    for(const auto& link : this->m_link_data) {
        if(link["level_id"] == level_id || link["link_id"] == level_id) {
            auto j_level_type = link["level_type"];
            level_link_type level_type = ll::str_to_level_link_type(link["level_type"]);
            auto j_link_type = link["link_type"];
            level_link_type link_type = ll::str_to_level_link_type(link["link_type"]);

            return std::optional<level_link_data>(level_link_data{
                .level_id = link["level_id"],
                .level_type = j_level_type.is_number() ? (level_link_type)int(j_level_type) : level_type,
                .link_id = link["link_id"],
                .link_type = j_link_type.is_number() ? (level_link_type)int(j_link_type) : link_type
            });
        }
    }

    return std::optional<level_link_data>();
}

const std::vector<level_link_data>& linking_manager::get_all_links() {
    if(this->m_link_data.empty()) {
        this->load_links();
    }

    if(!this->m_cached_data.empty()) {
        return this->m_cached_data;
    }

    auto parse_level_type = [](auto& object) -> level_link_type {
        level_link_type level_type = level_link_type::undefined;
        if(object.is_number()) level_type = (level_link_type)((int)object);
        else level_type = ll::str_to_level_link_type((std::string)object);
        return level_type;
    };

    for(const auto& link : this->m_link_data) {
        int level_id = link["level_id"];
        level_link_type level_type = parse_level_type(link["level_type"]);

        int link_id = link["link_id"];
        level_link_type link_type = parse_level_type(link["link_type"]);

        level_link_data data = {
            .level_id = level_id,
            .level_type = level_type,
            .link_id = link_id,
            .link_type = link_type
        };

        this->m_cached_data.push_back(data);
    }

    return this->m_cached_data;
}

bool linking_manager::is_info_layer() const {
    return this->m_is_info_layer;
}

void linking_manager::set_info_layer(bool v) {
    this->m_is_info_layer = v;
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
    std::string linked_lvls_str = MOD->getSavedValue<std::string>("level_links", "[]");
    this->m_link_data = nh::json::parse(linked_lvls_str);
}

void linking_manager::save_links() {
    MOD->setSavedValue("level_links", this->m_link_data.dump());
}

linking_manager& linking_manager::get() {
    static linking_manager instance;
    return instance;
}

}