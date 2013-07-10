#include "Gauge.h"

// インスタンスを作る
Gauge* Gauge::spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
{
    Gauge *pobSprite = new Gauge();
    if(pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame)) {
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}


// 初期化
void Gauge::init(float maxValue)
{
    this->maxValue = maxValue;
    value = maxValue;
    
    bar = CCSprite::create("gauge_red_bar.png");
    bar->setPosition(ccp(getContentSize().width / 2 - bar->getContentSize().width / 2,
                         getContentSize().height / 2));
    bar->setAnchorPoint(ccp(0.0, 0.5));
    addChild(bar);
    
    
    CCSprite *frame = CCSprite::create("gauge_frame.png");
    frame->setPosition(ccp(getContentSize().width / 2,
                           getContentSize().height / 2));
    addChild(frame);
}

void Gauge::init(float maxValue, ccColor3B color)
{
    init(maxValue);
    bar->setColor (color);
}


// 初期化したインスタンスを取得する
Gauge* Gauge::create(float maxValue)
{
    Gauge* Gauge = spriteWithSpriteFrame(CCSpriteFrame::create("gauge.png",
                                                               CCRectMake(0, 0, 480, 80)));
    Gauge->init(maxValue);
    return Gauge;
}


// 数値を減らす
void Gauge::decrease(float decreaseValue)
{
    value -= decreaseValue;
    if (value < 0) {
        value = 0;
    }
    bar->setScaleX(value / maxValue);
}


// 数を増やす
void Gauge::increase(float increaseValue)
{
    value += increaseValue;
    if (value > maxValue) {
        value = maxValue;
    }
    bar->setScaleX(value / maxValue);
}