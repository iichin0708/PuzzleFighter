#ifndef __BLOCKSPRITE__H__
#define __BLOCKSPRITE__H__

#include "cocos2d.h"
#include "Config.h"

class BlockSprite : public cocos2d::CCSprite
{
protected:
    const char* getBlockImageFileName(kBlock blockType);
    
public:
    CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);
    CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
    CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);
    CC_SYNTHESIZE(int, m_indexX, IndexX);
    CC_SYNTHESIZE(int, m_indexY, IndexY);
    CC_SYNTHESIZE(int, m_swapPartnerTag, SwapPartnerTag);
    CC_SYNTHESIZE(bool, m_isTouchFlag, IsTouchFlag);
    
    void initNextPos();
    void setNextPos(int nextPosX, int nextPosY);
    
    BlockSprite();
    virtual ~BlockSprite();
    virtual bool initWithBlockType(kBlock blockType);
    static BlockSprite* createWithBlockType(kBlock blockType);
};

#endif // __BLOCKSPRITE__H__