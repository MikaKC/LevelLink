#ifndef CUSTOM_LEVEL_CELL_H
#define CUSTOM_LEVEL_CELL_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

class level_link_modal;
class custom_level_cell : public cocos2d::CCLayerColor {
public:
    bool init(GJGameLevel* level, level_link_modal* layer, bool no_select_btn);
    void update_display(GJGameLevel* level);
    void on_select_btn_pressed(cocos2d::CCObject* sender);

    static custom_level_cell* create(GJGameLevel* level, level_link_modal* layer, bool no_select_btn = false);
private:
    GJGameLevel* m_level;
    level_link_modal* m_layer;
    CCSize m_layer_size;
    
    /* UI Elements */
    GJDifficultySprite* m_difficulty;
    CCLabelBMFont* m_name_text;
    ButtonSprite* m_select_btn_sprite;

    bool m_no_select_btn;
};

}

#endif