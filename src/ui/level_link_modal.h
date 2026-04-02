#ifndef LEVEL_LINK_MODAL_H
#define LEVEL_LINK_MODAL_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include "combo_box.h"

#include "../core/types.h"

#include "../utils/utils.h"
#include "../utils/button_sprite_utils.h"

#include "../hooks/level_info_layer_hook.h"
#include "../hooks/edit_level_layer_hook.h"

namespace ll {

class custom_level_cell;
class level_link_modal : public FLAlertLayer, public FLAlertLayerProtocol, public LevelManagerDelegate {
public:
    /* Hacky way of allowing this to interface with it's parent's layer */
    bool init(GJGameLevel* current_level, link_ui& layer_ui);
    void set_selected_level(GJGameLevel* level);

    static level_link_modal* create(GJGameLevel* current_level, link_ui& layer_ui);
    
    void update(float dt);

private:
    void setup_modal_ui();
    void setup_level_scroll();

    void on_link_btn_pressed(CCObject* obj);
    
    void on_editor_select();
    void on_online_select();
    
    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
    void loadLevelsFailed(char const* error) override;
    
    void update_display();
    void update_online_scroll();
    void update_editor_scroll();
    void update_selected_level();

    void populate_scroll(const std::vector<GJGameLevel*>& levels);
    void clear_scroll_layer();
    void move_scroll_to_top();

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn_2) override;
    
    void keyBackClicked();
    void keyDown(enumKeyCodes key, double p1);

    void onClose(CCObject*);

private:
    NineSlice* m_background;
    CCMenuItemSpriteExtra* m_close_btn;
    CCLabelBMFont* m_title;

    CCLabelBMFont* m_type_str;
    ScrollLayer* m_scroll_layer; 
    TextInput* m_id_search_input;
    LoadingCircleSprite* m_loading_circle;
    CCLabelBMFont* m_selected_cell_header;
    custom_level_cell* m_selected_cell;

    bool m_schedule_update_display;
    level_link_type m_type;
    link_ui* m_parent_ui;

    GJGameLevel* m_current_level;
    GJGameLevel* m_selected_level;

    /* Stupid hack */
    std::chrono::steady_clock::time_point m_last_search_time;
};

}

#endif