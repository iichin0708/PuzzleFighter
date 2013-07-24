#include "Timer.h"
#include "Gauge.h"
#include "GameScene.h"

GameScene* Timer::gameManager = NULL;

Timer::Timer(int limitTime) {
    preSec = 0;
    sec = 0;
    m_timer = 0;
    m_maxTime = limitTime;
}

Timer::~Timer() {
}

Timer* Timer::createTimer(int limitTime) {
    Timer *timer = new Timer(limitTime);
    timer->init();
    return timer;
}

bool Timer::init() {
    if (!CCNode::init()) {
        return false;
    }
    
    const char *str = CCString::createWithFormat("1:00")->getCString();
    CCLabelBMFont *timerLabel;
    timerLabel = CCLabelBMFont::create(str, "ui_time_number.fnt");
    timerLabel->setPosition(ccp(120, 595));
    gameManager->m_background->addChild(timerLabel, GameScene::kZOrderTimer, GameScene::kTagTimerNumber);
    
    return true;
}

void Timer::startTimer() {
   this->schedule(schedule_selector(Timer::countTimer));
}

void Timer::stopTimer() {
    this->unschedule(schedule_selector(Timer::countTimer));
}

void Timer::resetTimer() {
    m_timer = 0;
}

void Timer::countTimer(float time) {
    m_timer += time;
    sec = m_timer;
    Gauge *timeGuage = (Gauge*)gameManager->m_background->getChildByTag(GameScene::kTagTimerGauge);
    timeGuage->decrease(time);

    if (sec != preSec && sec <= m_maxTime) {
        preSec = sec;
        const char *timeStr;
        if ((int)m_maxTime - preSec < 60) {
             timeStr = CCString::createWithFormat("0:%02d", (int)m_maxTime - preSec)->getCString();
        } else {
            timeStr = CCString::createWithFormat("1:%02d", (int)m_maxTime - preSec - 60)->getCString();
        }
        CCLabelBMFont *timerLabel = (CCLabelBMFont*)gameManager->m_background->getChildByTag(GameScene::kTagTimerNumber);
        timerLabel->setCString(timeStr);
    }
}

void Timer::setGameManager(GameScene *gameInstance) {
    gameManager = gameInstance;
}
