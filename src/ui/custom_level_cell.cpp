#include "custom_level_cell.h"

#include "level_link_modal.h"

#include "../utils/button_sprite_utils.h"

namespace ll {
bool custom_level_cell::init(GJGameLevel* level, level_link_modal* layer, bool no_select_btn) {
    this->m_layer_size = CCSize { 200, 35 };
    if(no_select_btn) this->m_layer_size.width = 120;

    if(!cocos2d::CCLayerColor::initWithColor(ccColor4B { 0, 0, 0, 100 })) return false;
    this->setContentSize(this->m_layer_size);
    this->setAnchorPoint(CCPoint { 0.5f, 0.5f });
    
    this->m_level = level;
    this->m_layer = layer;
    this->m_no_select_btn = no_select_btn;

    const float border_padding = 5.f;

    /* I couldn't get GJGameLevel::m_difficulty to work with GJDifficultySprite::create */
    int level_icon = -1;
    if(level->m_stars) {
        if(level->m_stars == 2) level_icon = 1; /* Easy */
        else if(level->m_stars == 3) level_icon = 2; /* Normal */
        else if(level->m_stars > 3 && level->m_stars < 6) level_icon = 3; /* Hard */
        else if(level->m_stars > 5 && level->m_stars < 8) level_icon = 4; /* Harder */
        else if(level->m_stars > 7 && level->m_stars < 10) level_icon = 5; /* Insane */
        else if(level->m_stars > 9) level_icon = 6; /* Demon (although it changes below if demon) */
        else level_icon = 11; /* Auto levels are N/A, although GJDifficulty #0 is auto... */
    } else {
        /* Levels with no stars get the avg difficulty's face ig */
        level_icon = level->getAverageDifficulty();
    }

    this->m_difficulty = GJDifficultySprite::create(level_icon, GJDifficultyName::Short);

    if (level->m_autoLevel) {
        this->m_difficulty->updateDifficultyFrame(0, GJDifficultyName::Short);
    }

    if(level->m_demon) {
        this->m_difficulty->updateDifficultyFrame(
            level->demonIconForDifficulty((DemonDifficultyType)level->m_demonDifficulty), 
            GJDifficultyName::Short
        );
    }

    this->m_difficulty->updateFeatureStateFromLevel(level);
    this->m_difficulty->setAnchorPoint(CCPoint { 0.5f, 0.5f });
    this->m_difficulty->setScale(0.5f);
    
    this->m_difficulty->setPosition(CCPoint { 
        (this->m_difficulty->getScaledContentSize().width / 2) + border_padding, 
        this->m_layer_size.height / 2 
    });
    this->addChild(this->m_difficulty, 2);

    this->m_name_text = CCLabelBMFont::create(level->m_levelName.c_str(), "bigFont.fnt");
    this->m_name_text->setScale(0.5f);
    this->m_name_text->setAnchorPoint(CCPoint { 0.f, 0.5f });
    this->m_name_text->setPosition(CCPoint { 
        this->m_difficulty->getScaledContentSize().width + (border_padding * 2), 
        this->m_layer_size.height / 2 
    });
    
    this->m_select_btn_sprite = ButtonSprite::create(
        "Select", 
        "bigFont.fnt", 
        get_btn_texture_for_base(button_sprite_base::green)
    );
    this->m_select_btn_sprite->setScale(0.5f);

    if(!no_select_btn) {
        CCMenuItemSpriteExtra* select_btn = CCMenuItemSpriteExtra::create(
            this->m_select_btn_sprite,
            this,
            menu_selector(custom_level_cell::on_select_btn_pressed)
        );

        CCMenu* menu = CCMenu::create();
        menu->setContentSize(select_btn->getScaledContentSize());
        menu->setAnchorPoint({ 1.f, 0.5f });
        menu->addChild(select_btn);
        select_btn->setPosition(menu->getScaledContentSize() / 2);
        this->addChild(menu);

        menu->setPosition(CCPoint { 
            this->m_layer_size.width - menu->getScaledContentSize().width - border_padding, 
            this->m_layer_size.height / 4 
        });
    }

    CCSize real_name_size = this->m_name_text->getContentSize();
    CCSize curr_name_size = this->m_name_text->getScaledContentSize();
    float desired_name_width = this->m_layer_size.width - 
        (this->m_difficulty->getScaledContentSize().width + 
        (this->m_no_select_btn ? 0 : this->m_select_btn_sprite->getScaledContentSize().width) +
        (border_padding * 4));

    float desired_scale = this->m_name_text->getScale();

    if(curr_name_size.width > desired_name_width) 
        desired_scale = desired_name_width / real_name_size.width;
    
    this->m_name_text->setScale(desired_scale);

    this->addChild(this->m_name_text, 1);

    return true;
}

void custom_level_cell::update_display(GJGameLevel* level) {
    const float border_padding = 5.f;

    /* If you can get GJGameLevel::m_difficulty to work, you can change this */
    int level_icon = -1;
    if(level->m_stars) {
        if(level->m_stars == 2) level_icon = 1; /* Easy */
        else if(level->m_stars == 3) level_icon = 2; /* Normal */
        else if(level->m_stars > 3 && level->m_stars < 6) level_icon = 3; /* Hard */
        else if(level->m_stars > 5 && level->m_stars < 8) level_icon = 4; /* Harder */
        else if(level->m_stars > 7 && level->m_stars < 10) level_icon = 5; /* Insane */
        else if(level->m_stars > 9) level_icon = 6; /* Demon */
        else level_icon = 0;
    } else {
        /* Unrated levels get the average difficulty's face */
        level_icon = level->getAverageDifficulty();
    }

    this->m_difficulty->updateDifficultyFrame(level_icon, GJDifficultyName::Short);

    if (level->m_autoLevel) {
        this->m_difficulty->updateDifficultyFrame(0, GJDifficultyName::Short);
    }

    if(level->m_demon) {
        this->m_difficulty->updateDifficultyFrame(
            level->demonIconForDifficulty((DemonDifficultyType)level->m_demonDifficulty), 
            GJDifficultyName::Short
        );
    }

    this->m_difficulty->updateFeatureStateFromLevel(level);
    this->m_difficulty->setPosition(CCPoint { 
        (this->m_difficulty->getScaledContentSize().width / 2) + border_padding, 
        this->m_layer_size.height / 2 
    });
    
    this->m_name_text->setString(level->m_levelName.c_str());
    
    CCSize real_name_size = this->m_name_text->getContentSize();
    CCSize curr_name_size = this->m_name_text->getScaledContentSize();
    float desired_name_width = this->m_layer_size.width - 
    this->m_difficulty->getScaledContentSize().width -
    (this->m_no_select_btn ? 0 : this->m_select_btn_sprite->getScaledContentSize().width) -
    (border_padding * 4);
    
    float desired_scale = this->m_name_text->getScale();
    
    if(curr_name_size.width > desired_name_width) 
    desired_scale = desired_name_width / real_name_size.width;
    
    this->m_name_text->setScale(desired_scale);
    
}

void custom_level_cell::on_select_btn_pressed(cocos2d::CCObject* sender) {
    this->m_layer->set_selected_level(this->m_level);
}

custom_level_cell* custom_level_cell::create(GJGameLevel* level, level_link_modal* layer, bool no_select_btn) {
    auto* cell = new custom_level_cell;
    if(cell && cell->init(level, layer, no_select_btn)) {
        cell->autorelease();
        return cell;
    }
    return NULL;
}

}