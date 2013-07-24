#include "Gauge.h"
#include "GameScene.h"

GameScene* Gauge::gameManager = NULL;

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
void Gauge::init(float maxValue, const char *imgName)
{
    /*
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
    */
    fever = false;
    
    this->maxValue = maxValue;
    value = (int)maxValue;
    
    if (strcmp(imgName, "ui_bottom.png") == 0) {
        this->setPosition(ccp(this->getContentSize().width / 2, 600));
        this->setTag(kTimeFrame);
        
        bar = CCSprite::create("ui_time_meter.png");
        bar->setPosition(ccp(35, 2 * getContentSize().height / 3 - 2));
        bar->setAnchorPoint(ccp(0.0, 0.5));
        bar->setTag(kTimeBar);
        this->addChild(bar);
    } else if (strcmp(imgName, "ui_header.png") == 0) {
        this->setPosition(ccp(this->getContentSize().width / 2, 600 + getContentSize().height / 2));
        
        bar = CCSprite::create("ui_combo_meter.png");
        bar->setPosition(ccp(this->getContentSize().width - bar->getContentSize().width - 18, getContentSize().height / 2 - 2));
        bar->setAnchorPoint(ccp(0.0, 0.5));
        value = 0;
        bar->setScaleX(value / maxValue);
        this->addChild(bar);
    }
}

/*
void Gauge::init(float maxValue, ccColor3B color)
{
    init(maxValue);
    bar->setColor (color);
}
*/

// 初期化したインスタンスを取得する
Gauge* Gauge::create(float maxValue, const char *gaugeImgName)
{
    Gauge *gauge;
    if (strcmp(gaugeImgName, "ui_bottom.png") == 0) {
        gauge = spriteWithSpriteFrame(CCSpriteFrame::create(gaugeImgName,
                                                                CCRectMake(0, 0, 640, 106)));
    } else if (strcmp(gaugeImgName, "ui_header.png") == 0) {
        gauge = spriteWithSpriteFrame(CCSpriteFrame::create(gaugeImgName, CCRectMake(0, 0, 640, 242)));
    }
    gauge->init(maxValue, gaugeImgName);
    
    return gauge;
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
        if (!fever) {
            CCLog("fever");
            gameManager->startFever();
            fever = true;
        }
    }
    bar->setScaleX(value / maxValue);
}

void Gauge::setGameManager(GameScene *gameInstance) {
    gameManager = gameInstance;
}