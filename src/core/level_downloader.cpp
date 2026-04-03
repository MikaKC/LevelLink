#include "level_downloader.h"

namespace ll {

bool level_downloader::init(int level_id) {
    if(!CCNode::init()) return false;
    this->m_level_id = level_id;
    return true;
}

void level_downloader::levelDownloadFinished(GJGameLevel* level) {
    this->m_callback(level);
    this->release();
}

void level_downloader::levelDownloadFailed(int response) {
    geode::log::warn("Level download failed with response code: %d", response);
    this->release();
}

level_downloader* level_downloader::download() {
    this->retain();
    
    if(this->check_saved_levels()) return this;

    auto* glm = GameLevelManager::sharedState();
    glm->m_levelDownloadDelegate = this;
    glm->downloadLevel(this->m_level_id, false, 0);
    return this;
}

/* I'll look more into this, if there's a way to avoid searching through all saved */
bool level_downloader::check_saved_levels() {
    auto* glm = GameLevelManager::sharedState();
    cocos2d::CCArray* array = glm->getSavedLevels(false, 0);
    for(auto* level : CCArrayExt(array)) {
        GJGameLevel* real = static_cast<GJGameLevel*>(level);
        
        /* Since savedLevels are online only, we don't need to get the "real" level_id */
        if(real->m_levelID == this->m_level_id) {
            this->levelDownloadFinished(real);
            return true;
        }
    }
    return false;
}

level_downloader* level_downloader::set_callback(std::function<void(GJGameLevel*)> callback) {
    this->m_callback = callback;
    return this;
}

level_downloader* level_downloader::create(int level_id) {
    auto* downloader = new level_downloader;
    if(downloader && downloader->init(level_id)) {
        downloader->autorelease();
        return downloader;
    }
    return nullptr;
}


}