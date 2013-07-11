#ifndef __ScrollAndPickUp__Gauge__
#define __ScrollAndPickUp__Gauge__

#include "cocos2d.h"

using namespace cocos2d;

class Gauge : public CCSprite
{
private:
    // インスタンスを作る
    static Gauge* spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame);
    
    // バーのスプライト
    CCSprite *bar;
    
    // 現在の値
    float value;
    
    // 最大値
    float maxValue;
    
    // インスタンスの初期化
    void init(float maxValue);
    
    // インスタンスの初期化（色の設定付き）
    void init(float maxValue, ccColor3B color);
    
public:
    // 初期化したインスタンスを作る
    static Gauge* create(float maxValue);
    
    // 数値を減らす
    void decrease(float decreaseValue);
    
    // 数を増やす
    void increase(float increaseValue);
};

#endif /* defined(__ScrollAndPickUp__Gauge__) */
