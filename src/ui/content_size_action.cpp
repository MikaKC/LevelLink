#include "content_size_action.h"

namespace ll {

bool content_size_action::initWithDuration(float duration, const cocos2d::CCSize& s) {
    if(!cocos2d::CCActionInterval::initWithDuration(duration)) return false;

    this->end_cs_x = s.width;
    this->end_cs_y = s.height;

    return true;
}

void content_size_action::startWithTarget(cocos2d::CCNode *pTarget) {
    cocos2d::CCActionInterval::startWithTarget(pTarget);
    
    this->start_cs_x = pTarget->getContentSize().width;
    this->start_cs_y = pTarget->getContentSize().height;

    this->delta_cs_x = this->end_cs_x - this->start_cs_x;
    this->delta_cs_y = this->end_cs_y - this->start_cs_y;
}

void content_size_action::update(float time) {
    if(!m_pTarget) return;

    m_pTarget->setContentSize({ this->start_cs_x + (this->delta_cs_x * time), this->start_cs_y + (this->delta_cs_y * time) });
}

content_size_action* content_size_action::create(float duration, const cocos2d::CCSize& s) {
    content_size_action* csa = new content_size_action;
    if(csa && csa->initWithDuration(duration, s)) csa->autorelease();
    else return NULL;

    return csa;
}

}