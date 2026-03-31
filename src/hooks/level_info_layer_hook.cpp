#include "level_info_layer_hook.h"

#include "../core/linking_manager.h"
#include "../core/level_downloader.h"

#include "../ui/level_link_modal.h"

#include "../utils/link_ui_impl.h"

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#define MOD Mod::get()

namespace ll {

bool level_info_layer_hook::init(GJGameLevel* level, bool challenge) {
    if(!LevelInfoLayer::init(level, challenge)) return false;

    CCSize visible_size = CCDirector::get()->getVisibleSize();
    link_ui& ui = this->m_fields->m_ui;

    bool ret = ll::init(this, this->m_level, ui);

    ui.buttons_menu->setPosition(CCPoint {
        visible_size.width / 5,
        visible_size.height / 2
    });

    ui.create_link_btn->setTarget(this, menu_selector(level_info_layer_hook::on_modal_btn_pressed));
    ui.open_link_btn->setTarget(this, menu_selector(level_info_layer_hook::on_open_btn_pressed));
    ui.break_link_btn->setTarget(this, menu_selector(level_info_layer_hook::on_unlink_btn_pressed));

    return ret;
}

void level_info_layer_hook::on_modal_btn_pressed(CCObject* obj) {
    level_link_modal::create(this->m_level, this->m_fields->m_ui)->show();
}

void level_info_layer_hook::on_open_btn_pressed(CCObject* obj) {
    ll::on_open_btn_pressed(this->m_level);
}

void level_info_layer_hook::on_unlink_btn_pressed(CCObject* obj) {
    ll::on_break_btn_pressed(this->m_fields->m_ui);
}

}

