#include "level_link_modal.h"

#include "custom_level_cell.h"

#include "../core/linking_manager.h"

#include <cvolton.level-id-api/include/EditorIDs.hpp>

namespace ll {

/* Setups */
bool level_link_modal::init(GJGameLevel* current_level, link_ui& current_link_ui) {
    CCSize layer_size = CCSize { 400, 250 };
    CCTouchDispatcher::get()->registerForcePrio(this, 2);

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;

    auto winSize = CCDirector::get()->getWinSize();
    
    this->m_current_level = current_level;
    this->m_parent_ui = &current_link_ui;

    geode::log::debug("{}", this->m_current_level);

    this->m_mainLayer = CCLayer::create();
    this->addChild(this->m_mainLayer);

    this->m_background = NineSlice::create("GJ_square01.png", { 0, 0, 80, 80 });
    this->m_background->setContentSize(layer_size);
    this->m_background->setPosition(winSize.width / 2, winSize.height / 2);
    this->m_mainLayer->addChild(m_background);

    this->m_buttonMenu = CCMenu::create();
    this->m_buttonMenu->setZOrder(100);
    this->m_mainLayer->addChild(this->m_buttonMenu);

    this->m_mainLayer->ignoreAnchorPointForPosition(false);
    this->m_mainLayer->setPosition(winSize / 2);
    this->m_mainLayer->setContentSize(layer_size);
    this->m_mainLayer->setLayout(
        CopySizeLayout::create()
            ->add(this->m_buttonMenu)
            ->add(m_background)
    );

    this->m_title = CCLabelBMFont::create("", "bigFont.fnt");
    this->m_title->setZOrder(2);
    this->m_mainLayer->addChildAtPosition(this->m_title, Anchor::Top, {0, -20.f});

    this->m_title->setString("Add Link");
    this->m_title->limitLabelWidth(
        layer_size.width - 50.f, 
        0.7f, 
        .1f
    );

    this->setup_modal_ui();
    
    this->m_close_btn = CCMenuItemExt::createSpriteExtraWithFrameName(
        "GJ_closeBtn_001.png", 
        1.0f, 
        [this](auto btn) {
            this->onClose(btn);
        }
    );

    this->m_buttonMenu->addChildAtPosition(this->m_close_btn, geode::Anchor::TopLeft, { 10.f, -10.f });

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    this->scheduleUpdate();

    geode::cocos::handleTouchPriority(this); // Fix touch prios

    return true;
}

void level_link_modal::setup_modal_ui() {
    /* Link btn */
    ButtonSprite* link_spr = ButtonSprite::create(
        "Link", 
        "goldFont.fnt",
        get_btn_texture_for_base(button_sprite_base::green)
    );

    CCMenuItemSpriteExtra* link_btn = CCMenuItemSpriteExtra::create(
        link_spr,
        this,
        menu_selector(level_link_modal::on_link_btn_pressed)
    );

    this->m_buttonMenu->addChild(link_btn);
    link_btn->setPosition(CCPoint {
        m_buttonMenu->getScaledContentSize().width / 2,
        link_btn->getScaledContentHeight()
    });

    /* Link combo box */
    this->m_type = level_link_type::undefined;

    this->m_type_str = CCLabelBMFont::create("Level Type", "bigFont.fnt");
    this->m_mainLayer->addChild(this->m_type_str, 3);
    this->m_type_str->setScale(0.5f);

    this->m_type_str->setPosition(CCPoint {
        this->m_mainLayer->getScaledContentSize().width / 4 - 10.f,
        (3.f * (this->m_mainLayer->getScaledContentSize().height / 4.f))
    });
    
    combo_box* type_combo = combo_box::create();
    type_combo->add_option("Editor", (uint8_t)level_link_type::editor);
    type_combo->add_option("Online", (uint8_t)level_link_type::online);
    type_combo->set_combo_callback([this](uint8_t selected_id) {
        level_link_type selected_type = (level_link_type)selected_id;
        if(selected_type == this->m_type) return;

        switch(selected_type) {
            case level_link_type::editor:
                this->on_editor_select();
                break;
            case level_link_type::online:
                this->on_online_select();
                break;
            default:
                break;
        }
    });

    type_combo->setPosition(CCPoint {
        (this->m_mainLayer->getScaledContentSize().width / 4) - (type_combo->getScaledContentWidth() / 2) - 10.f,
        (3.f * (this->m_mainLayer->getScaledContentSize().height / 4.f)) - (type_combo->getScaledContentHeight()) - 10.f
    });

    this->m_mainLayer->addChild(type_combo, 5);

    /* Loading Circle */
    this->m_loading_circle = LoadingCircleSprite::create(1.f);
    this->m_mainLayer->addChild(this->m_loading_circle, 10);
    this->m_loading_circle->setPosition(CCPoint{
        2.8f*(this->m_mainLayer->getScaledContentSize().width / 4),
        (this->m_mainLayer->getScaledContentSize().height / 2) - 20.f
    });

    this->m_loading_circle->setScale(0.5f);
    this->m_loading_circle->setOpacity(128);

    this->m_loading_circle->setVisible(false);

    /* Id & Search input */
    this->m_schedule_update_display = false;
    this->m_id_search_input = TextInput::create(200, "Search...");
    this->m_id_search_input->setCallback([&](const std::string& ret) {
        auto now = std::chrono::steady_clock::now();
        
        this->m_loading_circle->setVisible(true);
        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - this->m_last_search_time).count() < 500) {
            this->m_schedule_update_display = true;
            return;
        }

        this->m_last_search_time = now;
        
        this->update_display();
    });

    this->m_id_search_input->setMaxCharCount(20);

    this->m_mainLayer->addChild(this->m_id_search_input);
    this->m_id_search_input->setPosition({
        2.8f * (this->m_mainLayer->getScaledContentSize().width / 4),
        3.f*(this->m_mainLayer->getScaledContentSize().height / 4) - 10.f
    });

    this->m_selected_cell = NULL;
    this->m_selected_cell_header = CCLabelBMFont::create("Selected:", "bigFont.fnt");
    this->m_selected_cell_header->setScale(0.5f);

    this->m_selected_cell_header->setPosition(CCPoint {
        this->m_mainLayer->getScaledContentSize().width / 4 - 10.f,
        (this->m_mainLayer->getScaledContentSize().height / 2) - 20.f
    });

    this->m_mainLayer->addChild(this->m_selected_cell_header, 2);

    this->setup_level_scroll();
}

void level_link_modal::setup_level_scroll() {        
    this->m_scroll_layer = ScrollLayer::create(
        { 200, 100 }
    );

    this->m_scroll_layer->setPosition(CCPoint {
        2.8f*(this->m_mainLayer->getScaledContentSize().width / 4) - (this->m_scroll_layer->getScaledContentSize().width / 2),
        (this->m_mainLayer->getScaledContentSize().height / 2) - (this->m_scroll_layer->getScaledContentSize().height / 2) - 20
    });

    /* Credit: https://github.com/Fleeym/jukebox/blob/main/jukebox/jukebox/ui/list/nong_list.cpp */
    this->m_scroll_layer->m_contentLayer->setLayout(geode::SimpleColumnLayout::create()
                                        ->setMainAxisDirection(AxisDirection::TopToBottom)
                                        ->setMainAxisAlignment(MainAxisAlignment::Start)
                                        ->setMainAxisScaling(AxisScaling::Grow)
                                        ->ignoreInvisibleChildren(false));

    this->m_mainLayer->addChild(m_scroll_layer, 5);

    this->on_editor_select(); /* Default to editor */
}


/* Button callbacks */
void level_link_modal::on_link_btn_pressed(CCObject* obj) {
    /* We've all written bad code... */
    if(!this->m_parent_ui) {
        geode::log::error("Failed to fetch ui!");
        return;
    }

    if(!this->m_selected_level) {
        FLAlertLayer::create(
            "Error", 
            "No level selected! Please select a level to link.", 
            "Ok"
        )->show();
        return;
    }

    auto id_for_type = [&](level_link_type type, GJGameLevel* level) -> int {
        if(type == level_link_type::editor) {
            return EditorIDs::getID(level);
        } else if(type == level_link_type::online) {
            return level->m_levelID;
        } else {
            return -1;
        }
    };
    
    GJGameLevel* current_level = this->m_current_level;
    level_link_type current_level_type = (this->m_current_level->m_levelType == GJLevelType::Editor) ? level_link_type::editor : level_link_type::online;
    int current_level_id = id_for_type(current_level_type, current_level);
    int selected_level_id = id_for_type(this->m_type, this->m_selected_level);
    
    bool is_duplicate = false;
    const std::vector<level_link_data>& all_entries = linking_manager::get().get_all_links();
    for(const auto& entry : all_entries) {
        if((entry.level_id == selected_level_id) || (entry.link_id == selected_level_id)) {
            is_duplicate = true;
        }
    }

    if(is_duplicate) {
        FLAlertLayer::create(
            "Error",
            "Link already exists!",
            "Ok"
        )->show();
        return;
    }
    
    level_link_data data {
        .level_id = current_level_id,
        .level_type = current_level_type,
        .link_id = selected_level_id,
        .link_type = this->m_type
    };

    linking_manager::get().create_link(data);
    linking_manager::get().set_current_data(data);
    ll::update_ui_draw(*this->m_parent_ui, true);

    FLAlertLayer::create(
        this,
        "Success", 
        "Created <cy>link</c> successfully!", 
        "Ok",
        NULL
    )->show();
}

void level_link_modal::on_editor_select() {
    if(m_type != level_link_type::editor) {
        this->update_editor_scroll();
    }
    
    m_type = level_link_type::editor;
}

void level_link_modal::on_online_select() {
    if(m_type != level_link_type::online) {
        this->update_online_scroll();
    }
    
    m_type = level_link_type::online;
}

/* Level Search */
void level_link_modal::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
    if(this->m_type != level_link_type::online) return;
    
    std::vector<GJGameLevel*> vlevels;
    for(auto* level : CCArrayExt(levels)) {
        GJGameLevel* l = static_cast<GJGameLevel*>(level);
        vlevels.push_back(l);
    }
    
    this->m_loading_circle->setVisible(false);
    this->populate_scroll(vlevels);
}

void level_link_modal::loadLevelsFailed(char const* error)  {
    if(this->m_type != level_link_type::online) return;
    
    geode::log::debug("Failed to fetch levels for query: {}", error);
}

/* State updates */
void level_link_modal::update(float dt) {
    /* Need to find a better way to handle this */
    if(this->m_schedule_update_display) {
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - this->m_last_search_time).count() >= 500) {
            this->update_display();
        }
    }
}

void level_link_modal::update_display() {
    this->m_schedule_update_display = false;
    if(this->m_type == level_link_type::editor) this->update_editor_scroll();
    else if(this->m_type == level_link_type::online) this->update_online_scroll();
}

void level_link_modal::update_online_scroll() {
    this->clear_scroll_layer();
    
    std::string text = this->m_id_search_input->getString();
    GJSearchObject* search_obj = GJSearchObject::create(SearchType::Search, text.c_str());
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->m_levelManagerDelegate = this;
    glm->getOnlineLevels(search_obj);
}

void level_link_modal::update_editor_scroll() {
    /* If I get time to refactor this, I will, but for now it works */
    this->clear_scroll_layer();
    
    CCArray* levels = LocalLevelManager::get()->m_localLevels;
    std::vector<GJGameLevel*> vlevels;
    for(auto* level : CCArrayExt(levels)) {
        GJGameLevel* l = static_cast<GJGameLevel*>(level);
        if(!l) continue;
        
        vlevels.push_back(l);
    }
    
    std::string text = this->m_id_search_input->getString();
    if(text.length() <= 0) {
        this->populate_scroll(vlevels);
        return;
    }

    std::vector<GJGameLevel*> sorted_levels = search_levels(vlevels, text);     
    this->m_loading_circle->setVisible(false);
    this->populate_scroll(sorted_levels);
}

void level_link_modal::update_selected_level() {
    if(!this->m_selected_cell) {
        this->m_selected_cell = custom_level_cell::create(this->m_selected_level, this, true);
        this->m_mainLayer->addChild(this->m_selected_cell);
        this->m_selected_cell->setPosition({
            (this->m_mainLayer->getScaledContentSize().width / 4) - (this->m_selected_cell->getScaledContentSize().width / 2) - 10.f,
            this->m_scroll_layer->getPositionY()
        });
        return;
    }

    this->m_selected_cell->update_display(this->m_selected_level);
}

/* Scroll modifications */
void level_link_modal::populate_scroll(const std::vector<GJGameLevel*>& levels) {
    if(levels.empty()) return;

    this->clear_scroll_layer();

    for(auto* obj : levels) {
        custom_level_cell* cl = custom_level_cell::create(obj, this);
        m_scroll_layer->m_contentLayer->addChild(cl);
    }

    this->m_scroll_layer->m_contentLayer->updateLayout();
    geode::cocos::handleTouchPriority(this);

    this->move_scroll_to_top();
}

void level_link_modal::clear_scroll_layer() {
    if(this->m_scroll_layer->m_contentLayer->getChildrenCount() > 0) {
        this->m_scroll_layer->m_contentLayer->removeAllChildrenWithCleanup(true);
    }
}

void level_link_modal::move_scroll_to_top() {
    m_scroll_layer->m_contentLayer->setPositionY(
        -m_scroll_layer->m_contentLayer->getScaledContentHeight() + 
        m_scroll_layer->getScaledContentHeight()
    );
}

/* Close handling */
void level_link_modal::FLAlert_Clicked(FLAlertLayer* layer, bool btn_2) { 
    this->onClose(NULL);
}

void level_link_modal::keyBackClicked() {
    this->onClose(nullptr);
}

void level_link_modal::keyDown(enumKeyCodes key, double p1) {
    if (key == KEY_Space) return;
    return FLAlertLayer::keyDown(key, p1);
}

void level_link_modal::onClose(CCObject*) {
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParent();
}

void level_link_modal::set_selected_level(GJGameLevel* level) {
    this->m_selected_level = level;
    this->update_selected_level();
}

level_link_modal* level_link_modal::create(GJGameLevel* current_level, link_ui& ui) {
    auto* modal = new level_link_modal;
    if(modal && modal->init(current_level, ui)) {
        modal->autorelease();
        return modal;
    }

    return NULL;
}

}