#ifndef CONTENT_SIZE_ACTION_H
#define CONTENT_SIZE_ACTION_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace ll {

/* Action to change the content size of a node over time */
class content_size_action : public cocos2d::CCActionInterval {
public:
    bool initWithDuration(float duration, const cocos2d::CCSize& s);
    
    virtual void startWithTarget(cocos2d::CCNode *pTarget);
    virtual void update(float time);

    static content_size_action* create(float duration, const cocos2d::CCSize& s);

private:
    float start_cs_x;
    float start_cs_y;

    float end_cs_x;
    float end_cs_y;

    float delta_cs_x;
    float delta_cs_y;
};

}

#endif