//
//  PowerUpSprite.cpp
//  PuzzleFighter
//
//  Created by GRENGE-MAC0015 on 13/07/24.
//
//

#include "PowerUpSprite.h"

USING_NS_CC;


PowerUpSprite::PowerUpSprite () {
    
}

PowerUpSprite::~PowerUpSprite() {
    
}

PowerUpSprite* PowerUpSprite::createPowerUpSprite(const char *imgName, int indexX, int indexY) {
    PowerUpSprite *pRet = new PowerUpSprite();
    if (pRet && pRet->initWithFile(imgName))
    {
        
        pRet->m_positionIndex = BlockSprite::PositionIndex(indexX, indexY);
        pRet->setPosition(pRet->getPosition(indexX, indexY));

        #pragma mark REFACTOR::画像の大きさにスケールを設定している
        pRet->setScale(2);

        pRet->toSprite = NULL;
        pRet->autorelease();
        
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool PowerUpSprite::initWithFile(const char *imgName) {
    if (!CCSprite::initWithFile(imgName)) {
        return false;
    }
    
    return true;
}

#pragma mark REFACTOR::ブロックの大きさがマジックナンバー
CCPoint PowerUpSprite::getPosition(int posIndexX, int posIndexY)
{
    return CCPoint((posIndexX + 0.5) * 90, (posIndexY + 0.5) * 90);
}
