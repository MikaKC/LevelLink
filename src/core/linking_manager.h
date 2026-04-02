#ifndef LINKING_MANAGER_H
#define LINKING_MANAGER_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "types.h"

namespace ll {

class linking_manager {
public:
    linking_manager();
    ~linking_manager() = default;

    void create_link(level_link_data data);
    void break_link(int level_id);

    std::optional<level_link_data> get_link(int level_id);

    const std::vector<level_link_data>& get_all_links();

    void set_current_data(const level_link_data& data);
    void clear_current_data();
    level_link_data& get_current_data();

    static linking_manager& get();

private:
    void load_links();
    void save_links();

private:
    ll::level_link_data m_current_data;
    std::vector<level_link_data> m_data;
};

}

#endif