//
//  PowerUpSprite.h
//  PuzzleFighter
//
//  Created by GRENGE-MAC0015 on 13/07/24.
//
//

#ifndef __PuzzleFighter__PowerUpSprite__
#define __PuzzleFighter__PowerUpSprite__

#include <cocos2d.h>
#include "BlockSprite.h"

class PowerUpSprite : public cocos2d::CCSprite {
protected:

public:
    PowerUpSprite();
    virtual ~PowerUpSprite();
    
    BlockSprite::PositionIndex m_positionIndex;

    virtual bool initWithFile(const char *imgName);
    static PowerUpSprite* createPowerUpSprite(const char *imgName, int indexX, int indexY);
    
    BlockSprite *toSprite;
    
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);
};

#endif /* defined(__PuzzleFighter__PowerUpSprite__) */
