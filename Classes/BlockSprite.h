#ifndef __BLOCKSPRITE__H__
#define __BLOCKSPRITE__H__

#include "cocos2d.h"
#include "Config.h"

#define BLOCK_BASE_TAG 10000

//#include "GameScene.h"
class GameScene;

class BlockSprite : public cocos2d::CCSprite
{
protected:
    static GameScene* gameManager;
    
    const char* getBlockImageFileName(kBlock blockType);
    
public:
    enum kDeleteState
    {
        kNotDelete = 0,
        kDeleteThree = 3,
        kDeleteFour,
        kDeleteFive,
    };
    
    CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);
    CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
    CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);
    CC_SYNTHESIZE_READONLY(int, m_blockSize, BlockSize);
    CC_SYNTHESIZE(int, m_indexX, IndexX);
    CC_SYNTHESIZE(int, m_indexY, IndexY);
    CC_SYNTHESIZE(int, m_swapPartnerTag, SwapPartnerTag);
    CC_SYNTHESIZE(bool, m_isTouchFlag, IsTouchFlag);
    
    CC_SYNTHESIZE(kDeleteState, m_deleteState, DeleteState);

    CC_SYNTHESIZE(BlockSprite*, m_partnerBlock, PartnerBlock)
    
    
    enum kBlockState
    {
        kStopping,
        kChanging,
        kRechanging,
        kPreDropping,
        kDropping,
        kDeleting,
    };
    
    
    struct PositionIndex
    {
        PositionIndex() {
            x = -1;
            y = -1;
        }
        
        PositionIndex(int x1, int y1)
        {
            x = x1;
            y = y1;
        }
        
        int x;
        int y;
    };
    
#pragma mark m_bockSize[s]にしてある
    float m_blockSizes;
    PositionIndex m_positionIndex;
    PositionIndex m_prePositionIndex;
    PositionIndex m_postPositionIndex;
    
    kDeleteState deleteState;
    
    void initNextPos();
    void setNextPos(int nextPosX, int nextPosY);
    
    BlockSprite();
    virtual ~BlockSprite();
    virtual bool initWithBlockType(kBlock blockType);
    static BlockSprite* createWithBlockType(kBlock blockType, int indexX, int indexY);
    static void setGameManager(GameScene *gameInstance);

    void setPositionIndex(int indexX, int indexY);
//    PositionIndex getPositionIndex();
    cocos2d::CCPoint getBlockPosition(int indexX, int indexY);
    int getTag(int posIndexX, int posIndexY);
    
    int m_blockState;
    
    void moveBlock();
    
    void changePosition();
    
    void changePositionFinished();
    
    bool isMakeChain();
    
    void removeSelf();
    
    void removeSelfAnimation();
    
    void dropBlock();
    
    void dropFinished();
    
    void setNewBlock();
    
    void setNewPosition();
    
    void animExplosion(float animTime);
    
    void animSplash(float animTime);
    
    void animJump(float animTime);
    
    void animFloat(float animTime);
    
    void animHopSplash(float animTime);
    
    void animDrop(float animTime);
    
    void animLowJumpAndDrop(float animTime);

    void animSlash(float animTime);
    
    void animImage(float animTime);
    
    void animPress(float animTime);
    
    static bool doubleDelete;
};

#endif // __BLOCKSPRITE__H__