#ifndef LEVEL_DOWNLOADER_H
#define LEVEL_DOWNLOADER_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

class level_downloader : public cocos2d::CCNode, public LevelDownloadDelegate {
public:
    bool init(int level_id);

    void levelDownloadFinished(GJGameLevel* level) override;
    void levelDownloadFailed(int response) override;

    /* Returns current instance */
    level_downloader* download();
    level_downloader* set_callback(std::function<void(GJGameLevel*)> callback);

    static level_downloader* create(int level_id);

private:
    /* Returns true if level is found */
    bool check_saved_levels();

private:
    int m_level_id;
    std::function<void(GJGameLevel*)> m_callback;
};

}

#endif