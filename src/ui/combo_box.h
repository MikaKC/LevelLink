#ifndef COMBO_BOX_H
#define COMBO_BOX_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

using combo_callback = std::function<void(uint16_t)>;

typedef struct combo_label {
    std::string title;
    CCMenuItemSpriteExtra* label;
} combo_label;

/* Combo box node
    * Add options with add_option, taking a label string and an ID
    * Set a callback with set_combo_callback, the callback will return the ID of the selected option
*/
class combo_box : public cocos2d::CCLayer {
private:
    geode::NineSlice* combo_bg;

    cocos2d::CCLabelBMFont* combo_title;
    cocos2d::CCMenu* combo_menu;
    cocos2d::CCSprite* combo_arrow;
    
    bool is_open;

    bool detected_body_touch;

    combo_callback label_callback;
    std::unordered_map<uint8_t, combo_label> combo_labels;
private:
    void on_child_click(cocos2d::CCObject*);
    void on_body_click();

    void set_combo_title(const char*);

    void create_option(const char*, uint8_t);

    bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void ccTouchMoved(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void ccTouchEnded(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    
    void registerWithTouchDispatcher() override;

    float get_combined_label_heights();

public:
    bool init();
    void set_combo_callback(combo_callback cb);
    void add_option(std::string_view, uint8_t);

    void set_state(uint8_t selected_id);

    void onExit() override;

    static combo_box* create();
};

}

#endif