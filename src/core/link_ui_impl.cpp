#include "link_ui_impl.h"

#include "linking_manager.h"
#include "level_downloader.h"

#include "node_ids.h"

#include "../utils/utils.h"

#include <cvolton.level-id-api/include/EditorIDs.hpp>

namespace ll {

bool init(CCLayer* self, GJGameLevel* level, ll::link_ui& ui) {
    int level_id = ll::utils::get_real_level_id(level);
    if(level_id == -1) return false;

    bool found_link = false;
    level_link_data link_data = ll::load_data(level_id, found_link);
    linking_manager::get().set_current_data(link_data);

    ll::create_ui(self, ui);
    ll::layout_ui(ui);

    self->addChild(ui.buttons_menu, 10);

    ll::update_ui_draw(ui, found_link);

    return true;
}

level_link_data load_data(int self_id, bool& found_data) {
    std::optional<level_link_data> dirty_data = linking_manager::get().get_link(self_id);
    level_link_data data = dirty_data.value_or(
        level_link_data {
            .level_id = -1,
            .level_type = level_link_type::undefined,
            .link_id = -1,
            .link_type = level_link_type::undefined
        }
    );

    found_data = dirty_data != std::nullopt;

    return data;
}

void create_ui(CCLayer* self, link_ui& btns) {
    bool use_text_buttons = Mod::get()->getSettingValue<bool>("use-text-buttons");
    
    auto create_aliased_btn = [&](const char* sprite, const char* alias) -> CCNode* {
        CCNode* out = nullptr;
        if(use_text_buttons) out = create_circle_btn(alias);
        else out = CCSprite::createWithSpriteFrameName(sprite);

        if(!out) {
            geode::log::error("Failed to get sprite for button!");
            return nullptr;
        }

        return out;
    };

    CCNode* modal_spr = create_aliased_btn("gj_linkBtn_001.png", "Link");
    if(!use_text_buttons) modal_spr->setScale(1.25f);

    btns.create_link_btn = CCMenuItemSpriteExtra::create(
        modal_spr,
        self,
        NULL
    );

    btns.create_link_btn->setID(LL_ID_LINK_BTN);

    CCNode* open_spr = create_aliased_btn("GJ_playBtn2_001.png", "Open");
    if(!use_text_buttons) open_spr->setScale(0.6f);

    btns.open_link_btn = CCMenuItemSpriteExtra::create(
        open_spr,
        self,
        NULL
    );
    btns.open_link_btn->setID(LL_ID_OPEN_BTN);

    CCNode* unlink_spr = create_aliased_btn("gj_linkBtnOff_001.png", "Unlk");
    
    btns.break_link_btn = CCMenuItemSpriteExtra::create(
        unlink_spr,
        self,
        NULL
    );
    btns.break_link_btn->setID(LL_ID_BREAK_BTN);
    
    btns.buttons_menu = CCMenu::create();
    btns.buttons_menu->setID(LL_ID_BTN_MENU);

    btns.buttons_menu->addChild(btns.create_link_btn);
    btns.buttons_menu->addChild(btns.open_link_btn);
    btns.buttons_menu->addChild(btns.break_link_btn);
}

void layout_ui(link_ui& btns) {
    bool use_text_buttons = Mod::get()->getSettingValue<bool>("use-text-buttons");

    float btn_width = btns.open_link_btn->getScaledContentWidth();
    float btn_height = btns.open_link_btn->getScaledContentHeight();
    
    float menu_width = btn_width;
    float menu_height = btn_height * 2;

    btns.buttons_menu->setContentSize(CCSize {
        menu_width,
        menu_height
    });

    btns.create_link_btn->setPosition(CCPoint {
        btns.buttons_menu->getScaledContentSize().width / 2,
        btns.buttons_menu->getScaledContentSize().height / 2
    });

    if(use_text_buttons) {
        float btn_x = menu_width / 2;
        float btn_y = menu_height / 4;

        btns.open_link_btn->setPosition(CCPoint {
            btn_x,
            btn_y * 3.f
        });

        btns.break_link_btn->setPosition(CCPoint {
            btn_x,
            btn_y,
        });
    } else {
        const float middle_padding = 5.f;
        btns.open_link_btn->setPosition(CCPoint {
            menu_width * 0.5f,
            menu_height * 0.5f + (btns.break_link_btn->getScaledContentHeight() * 0.5f) + middle_padding 
        });

        btns.break_link_btn->setPosition(CCPoint {
            menu_width * 0.5f,
            menu_height * 0.5f - (btns.break_link_btn->getScaledContentHeight() * 0.5f) - middle_padding
        });
    }
}

bool _ui_is_valid(const link_ui& btns) {
    if(btns.open_link_btn == nullptr) return false;
    else if(btns.break_link_btn == nullptr) return false;
    else if(btns.create_link_btn == nullptr) return false;
    return true;
}

void update_ui_draw(const link_ui& btns, bool found_link) {
    if(!_ui_is_valid(btns)) return;
    
    btns.open_link_btn->setEnabled(found_link);
    btns.open_link_btn->setVisible(found_link);

    btns.break_link_btn->setEnabled(found_link);
    btns.break_link_btn->setVisible(found_link);
    
    btns.create_link_btn->setEnabled(!found_link);
    btns.create_link_btn->setVisible(!found_link);
}

void on_open_btn_pressed(GJGameLevel* self) {
    level_link_data data = linking_manager::get().get_current_data();
    
    int self_id = ll::utils::get_real_level_id(self); 
    bool is_link = (data.link_id == self_id);
    
    level_link_type target_type = is_link ? data.level_type : data.link_type;
    int target_id = is_link ? data.level_id : data.link_id;

    if(target_type == level_link_type::undefined) {
        FLAlertLayer::create(
            "Error", 
            "This link is invalid!", 
            "Ok"
        )->show();
        return;
    }

    if(target_type == level_link_type::editor) {
        GJGameLevel* level = EditorIDs::getLevelByID(target_id);
        if(!level) {
            FLAlertLayer::create(
                "Error", 
                "Failed to find linked level in editor!", 
                "Ok"
            )->show();
            return;
        }

        CCDirector::sharedDirector()->replaceScene(
            CCTransitionFade::create(
                0.5f, 
                EditLevelLayer::scene(level)
            )
        );
    } else {
        level_downloader::create(target_id)->set_callback(
            [](GJGameLevel* level) {
                if(!level) {
                    FLAlertLayer::create(
                        "Error", 
                        "Failed to download linked level!", 
                        "Ok"
                    )->show();
                    return;
                }

                CCDirector::sharedDirector()->replaceScene(
                    CCTransitionFade::create(
                        0.5f, 
                        LevelInfoLayer::scene(level, false)
                    )
                );
            }
        )->download();
    }
}

void on_break_btn_pressed(const link_ui& btns) {
    level_link_data data = linking_manager::get().get_current_data();
    linking_manager::get().break_link(data.level_id);
    linking_manager::get().clear_current_data();

    FLAlertLayer::create(
        "Success", 
        "Broke <cy>link</c> successfully!", 
        "Ok"
    )->show();

    ll::update_ui_draw(btns, false);
}

CircleButtonSprite* create_circle_btn(const char* text) {
    return CircleButtonSprite::create(
        CCLabelBMFont::create(text, "bigFont.fnt"),
        CircleBaseColor::Green,
        CircleBaseSize::Medium
    );
}

}