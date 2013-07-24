#include "Timer.h"
#include "Gauge.h"
#include "GameScene.h"

GameScene* Timer::gameManager = NULL;

Timer::Timer(int limitTime) {
    preSec = 0;
    sec = 0;
    m_timer = 0;
    m_maxTime = limitTime;
    showAlertFlag = false;
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

// 時間を管理する.
void Timer::countTimer(float time) {
    m_timer += time;
    sec = m_timer;
    Gauge *timeGuage = (Gauge*)gameManager->m_background->getChildByTag(GameScene::kTagTimerGauge);
    timeGuage->decrease(time);

    //1秒経過毎にこのif文に入る
    if (sec != preSec && sec <= m_maxTime)
    {
        preSec = sec;
        const char *timeStr;
        
        // 残り時間が60秒以下の時のタイムラベルの表示方法
        if ((int)m_maxTime - preSec < 60) {
             timeStr = CCString::createWithFormat("0:%02d", (int)m_maxTime - preSec)->getCString();
            //残り時間が5秒を切ったらアラートを表示
            if ((int)m_maxTime - preSec < 5 && !showAlertFlag) {
                CCLog("showAlert");
                gameManager->showAlert();
                showAlertFlag = true;
            }
        }
        // 残り時間が60秒以上の時のタイムラベルの表示方法
        else if(60 <= (int)m_maxTime -preSec) {
            timeStr = CCString::createWithFormat("1:%02d", (int)m_maxTime - preSec - 60)->getCString();
        }
        CCLabelBMFont *timerLabel = (CCLabelBMFont*)gameManager->m_background->getChildByTag(GameScene::kTagTimerNumber);
        timerLabel->setCString(timeStr);
    }
    // ゲーム終了時間を過ぎた場合
    else if ((int)m_maxTime <= m_timer)
    {
        gameManager->timeUp();
        stopTimer();
    }
}

void Timer::setGameManager(GameScene *gameInstance) {
    gameManager = gameInstance;
}
