#ifndef LINK_UI_IMPL_H
#define LINK_UI_IMPL_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "../core/types.h"

/* I'll update this setup once I have time */
namespace ll {

typedef struct link_ui {
    CCMenuItemSpriteExtra* create_link_btn;
    CCMenuItemSpriteExtra* open_link_btn;
    CCMenuItemSpriteExtra* break_link_btn;

    CCMenu* buttons_menu;
} link_ui;

enum class link_ui_orientation {
    portrait = 0,
    landscape = 1
};

bool init(CCLayer* self, GJGameLevel* level, ll::link_ui& ui);
level_link_data load_data(int self_id, bool& found_data);

void create_ui(CCLayer* self, link_ui& btns);
void layout_ui(link_ui& btns, link_ui_orientation orientation);

void update_ui_draw(const link_ui& btns, bool found_data);
void on_open_btn_pressed(GJGameLevel* self);
void on_break_btn_pressed(const link_ui& btns);

CircleButtonSprite* create_circle_btn(const char* text);

}

#endif