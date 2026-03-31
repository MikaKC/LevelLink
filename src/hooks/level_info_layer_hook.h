#ifndef LEVEL_INFO_LAYER_HOOK_H
#define LEVEL_INFO_LAYER_HOOK_H

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
using namespace geode::prelude;

#include "../core/types.h"
#include "../utils/link_ui_impl.h"

namespace ll {

class $modify(level_info_layer_hook, LevelInfoLayer) {
    struct Fields {
        ll::link_ui m_ui;
    };
    
    bool init(GJGameLevel* level, bool challenge);

    void on_modal_btn_pressed(CCObject* obj);
    void on_open_btn_pressed(CCObject* obj);
    void on_unlink_btn_pressed(CCObject* obj);
};

}

#endif