#include "edit_level_layer_hook.h"
#include "../ui/level_link_modal.h"

#include <cvolton.level-id-api/include/EditorIDs.hpp>

namespace ll {

bool edit_level_layer_hook::init(GJGameLevel* level) {
    if(!EditLevelLayer::init(level)) return false;

    CCSize visible_size = CCDirector::get()->getVisibleSize();
    link_ui& ui = this->m_fields->m_ui;

    bool ret = ll::init(this, this->m_level, this->m_fields->m_ui);

    ui.buttons_menu->removeFromParent();

    CCMenu* level_edit_menu = static_cast<CCMenu*>(this->getChildByID("level-edit-menu"));
    level_edit_menu->addChild(ui.buttons_menu);
    level_edit_menu->updateLayout();

    ui.create_link_btn->setTarget(this, menu_selector(edit_level_layer_hook::on_modal_btn_pressed));
    ui.open_link_btn->setTarget(this, menu_selector(edit_level_layer_hook::on_open_btn_pressed));
    ui.break_link_btn->setTarget(this, menu_selector(edit_level_layer_hook::on_unlink_btn_pressed));
    
    return ret;
}

void edit_level_layer_hook::on_open_btn_pressed(CCObject* obj) {
    ll::on_open_btn_pressed(this->m_level);
}

void edit_level_layer_hook::on_modal_btn_pressed(CCObject* obj) {
    /* UI must persist since it's stored as a pointer in level_link_modal */
    level_link_modal::create(this->m_level, this->m_fields->m_ui)->show();
}


void edit_level_layer_hook::on_unlink_btn_pressed(CCObject* obj) {
    ll::on_break_btn_pressed(this->m_fields->m_ui);
}

}


