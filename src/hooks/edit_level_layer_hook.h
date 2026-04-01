#ifndef EDIT_LEVEL_LAYER_HOOK_H
#define EDIT_LEVEL_LAYER_HOOK_H

#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
using namespace geode::prelude;

#include "../core/types.h"
#include "../core/link_ui_impl.h"

namespace ll {

class $modify(edit_level_layer_hook, EditLevelLayer) {
    struct Fields {
        ll::link_ui m_ui;
    };
    
    bool init(GJGameLevel*);
    
    void on_modal_btn_pressed(CCObject* obj);
    void on_open_btn_pressed(CCObject* obj);
    void on_unlink_btn_pressed(CCObject* obj);
};

}

#endif