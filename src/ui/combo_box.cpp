#include "combo_box.h"
#include "content_size_action.h"

namespace ll {

bool combo_box::init() {
    if(!cocos2d::CCLayer::init()) return false;

    this->detected_body_touch = false;
    this->is_open = false;
    this->label_callback = NULL;
    this->combo_labels = {};

    this->combo_bg = geode::NineSlice::create("square02b_001.png", { 0, 0, 80, 80 });
    this->combo_bg->setOpacity(100u);
    this->combo_bg->setAnchorPoint({ 0.5f, 1.0f });
    this->combo_bg->setContentSize(CCSize {
        200.f, 60.f
    });
    this->combo_bg->setScale(0.5f);
    this->combo_bg->setColor(_ccColor3B { 0, 0, 0 });
    
    this->setContentSize(this->combo_bg->getScaledContentSize());
    
    this->combo_title = cocos2d::CCLabelBMFont::create("None", "bigFont.fnt");
    this->combo_title->setAnchorPoint({ 0.f, 0.5f });

    const float maximum_width = this->getScaledContentWidth() / 2.f;
    this->combo_title->setScale(maximum_width / this->combo_title->getContentWidth());
    
    this->combo_bg->setPosition({ this->getScaledContentWidth() / 2, this->getScaledContentHeight() });
    this->combo_title->setPosition(CCPoint {
        10.f,
        this->getScaledContentHeight() / 2.f    
    });

    this->addChild(this->combo_title, 1);
    this->addChild(this->combo_bg, 0);

    this->combo_arrow = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    this->combo_arrow->setPosition(CCPoint {
        this->getScaledContentWidth() - 20.f,
        this->getScaledContentHeight() / 2.f
    });
    this->combo_arrow->setScale(0.5f);
    this->addChild(this->combo_arrow, 1);
    
    this->combo_menu = CCMenu::create();
    this->combo_menu->setPosition(CCPoint { 0, -(this->getScaledContentHeight() / 4) });
    this->combo_menu->setContentSize(this->getScaledContentSize());
    
    this->addChild(this->combo_menu);

    this->setTouchEnabled(true);

    return true;
}

void combo_box::set_combo_callback(combo_callback cb) {
    this->label_callback = cb;
}

void combo_box::on_child_click(cocos2d::CCObject* sender) {
    if(!this->is_open) return; 
    
    CCMenuItemSpriteExtra* label = static_cast<CCMenuItemSpriteExtra*>(sender);
    cocos2d::CCLabelBMFont* label_spr = static_cast<cocos2d::CCLabelBMFont*>(label->getNormalImage());
    
    if(!label_spr)  {
        geode::log::error("Combo box item is not a label!");
        return;
    }

    this->set_combo_title(label_spr->getString());
    if(this->label_callback) this->label_callback(sender->getTag());

    this->on_body_click();
}

float combo_box::get_combined_label_heights() {
    float height = 0;
    for(auto& pair : this->combo_labels) {
        height += pair.second.label->getScaledContentHeight();
        height += pair.second.label->getScaledContentHeight() * 0.5f; 
    }
    return height;
}

void combo_box::on_body_click() {
    if(this->combo_labels.size() == 0) return;
    if(this->combo_bg->numberOfRunningActions() > 0) this->combo_bg->stopAllActions();

    float combos_height = 0;
    if(!this->is_open) combos_height = this->get_combined_label_heights();
    float desired_height = (this->getScaledContentHeight() + combos_height) * 2;

    cocos2d::CCSize size = { (this->getScaledContentWidth()*2) * (this->is_open ? 1.f : 1.1f), desired_height };
    content_size_action* action = content_size_action::create(1.f, size);
    cocos2d::CCEaseElasticOut* content_size = cocos2d::CCEaseElasticOut::create(action, 0.6);

    if(this->combo_bg) this->combo_bg->runAction(content_size);

    if(!this->is_open) {
        for(auto& pair : this->combo_labels) {
            pair.second.label->runAction(CCFadeTo::create(0.3f, 255));
            pair.second.label->setEnabled(true);
        }
        this->combo_arrow->runAction(CCEaseInOut::create(cocos2d::CCRotateTo::create(0.3f, 180.f), 2.0f));
    } else {
        for(auto& pair : this->combo_labels) {
            pair.second.label->runAction(CCFadeTo::create(0.3f, 0));
            pair.second.label->setEnabled(false);
        }
        this->combo_arrow->runAction(CCEaseInOut::create(cocos2d::CCRotateTo::create(0.3f, 0), 2.0f));
    }

    this->is_open = !this->is_open;
}

void combo_box::set_combo_title(const char* title) {
    this->combo_title->setCString(title);

    const float maximum_width = this->getScaledContentWidth() / 2.f;
    this->combo_title->setScale(maximum_width / this->combo_title->getContentWidth());
}

void combo_box::create_option(const char* title, uint8_t id) {
    cocos2d::CCLabelBMFont* label_spr = cocos2d::CCLabelBMFont::create(title, "bigFont.fnt");
    const float maximum_width = this->getScaledContentWidth() / 2.f;
    label_spr->setScale(maximum_width / label_spr->getContentWidth());

    CCMenuItemSpriteExtra* label = CCMenuItemSpriteExtra::create(label_spr, this, menu_selector(combo_box::on_child_click));
    this->combo_labels.insert(std::pair(id, combo_label { title, label }));
    
    label->setTag(id);
    label->setOpacity(0);
    label->setEnabled(false);
    
    float y_pos = 0;
    if(this->combo_labels.size() > 1) {
        CCMenuItemSpriteExtra* last_label = this->combo_labels[this->combo_labels.size() - 2].label;
        y_pos = (last_label->getPositionY() - (last_label->getScaledContentHeight() / 2)) - label->getScaledContentHeight() * 1.1f;
    }

    label->setPosition(CCPoint {
        this->getScaledContentWidth() / 2,
        y_pos
    });

    this->combo_menu->addChild(label);
}

bool combo_box::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent*) {
    cocos2d::CCPoint touch_location = touch->getLocation();
    touch_location = this->getParent()->convertToNodeSpace(touch_location);

    if(this->boundingBox().containsPoint(touch_location)) {
        detected_body_touch = true;
        return true;
    }

    return false;
}

void combo_box::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent*) { }

void combo_box::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent*) {
    if(detected_body_touch) {
        detected_body_touch = false;

        cocos2d::CCPoint touch_location = touch->getLocation();
        touch_location = this->getParent()->convertToNodeSpace(touch_location);

        if(this->boundingBox().containsPoint(touch_location)) this->on_body_click();
    }
}

void combo_box::add_option(std::string_view title, uint8_t id) {
    this->create_option(title.data(), id);
    
    if(!this->combo_labels.empty() && this->combo_labels.size() <= 1) {
        this->set_combo_title(title.data());
    }
}

void combo_box::set_state(uint8_t selected_id) {
    if(combo_labels.empty()) return;

    auto val = this->combo_labels.find(selected_id);
    if(val == this->combo_labels.end()) return;

    this->set_combo_title((*val).second.title.c_str());
}

void combo_box::onExit() {
    this->setTouchEnabled(false);
    cocos2d::CCLayer::onExit();
}

void combo_box::registerWithTouchDispatcher() {
    cocos2d::CCTouchDispatcher* dispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority - 1, true);
}

combo_box* combo_box::create() {
    combo_box* node = new combo_box;
    if(node && node->init()) node->autorelease();
    else return NULL;

    return node;
}

}