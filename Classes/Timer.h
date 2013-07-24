#ifndef __PuzzleFighter__Timer__
#define __PuzzleFighter__Timer__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class GameScene;

class Timer : public cocos2d::CCNode {
private:
    static GameScene* gameManager;
    
    Timer(int limitTime);
    
    ~Timer();
    
    int preSec;
    
    int sec;
    
    void countTimer(float time);
    
public:
    CC_SYNTHESIZE_READONLY(float, m_timer, Time);
    CC_SYNTHESIZE(float, m_maxTime, MaxTime);
    
    
    // 初期化のメソッド
    virtual bool init();
    
    static void setGameManager(GameScene *gameInstance);

    static Timer* createTimer(int limitTime);
    
    // タイマーを動かす
    void startTimer();
    
    // タイマーを停止する
    void stopTimer();
    
    // タイマーをリセットする
    void resetTimer();
    
};


#endif /* defined(__PuzzleFighter__Timer__) */
