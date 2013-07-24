#include "BlockSprite.h"
#include "GameScene.h"
#include "PowerUpSprite.h"

GameScene* BlockSprite::gameManager = NULL;
bool BlockSprite::doubleDelete = false;


BlockSprite::BlockSprite()
{
    initNextPos();
    m_swapPartnerTag = -1;
    m_blockState = kStopping;
    m_isTouchFlag = true;
    m_blockLevel = 0;
    m_isLevelUp = false;
    starTag = 1000;
    collectSprite = NULL;
}

BlockSprite::~BlockSprite()
{
}

void BlockSprite::setGameManager(GameScene *gameInstance) {
    gameManager = gameInstance;
}

BlockSprite* BlockSprite::createWithBlockType(kBlock blockType, int indexX, int indexY)
{
    BlockSprite *pRet = new BlockSprite();
    if (pRet && pRet->initWithBlockType(blockType))
    {
        pRet->m_blockSizes = pRet->getContentSize().height;
        pRet->m_positionIndex = PositionIndex(indexX, indexY);
        pRet->m_prePositionIndex = pRet->m_positionIndex;
        pRet->deleteState = kNotDelete;
        
        pRet->setPartnerBlock(NULL);
        pRet->autorelease();
       
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool BlockSprite::initWithBlockType(kBlock blockType)
{
    if (!CCSprite::initWithFile(getBlockImageFileName(blockType))) {
        return false;
    }
    
    m_blockType = blockType;
    
    return true;
}

const char* BlockSprite::getBlockImageFileName(kBlock blockType)
{
    switch (blockType) {
        case kBlockPig:
            if (m_blockLevel == 0) {
                return "block_1_1.png";
            } else if (m_blockLevel == 1) {
                return "block_1_3.png";
            } else if (m_blockLevel == 2) {
                return "block_1_4.png";
            }
            
        case kBlockHumanRed:
            if (m_blockLevel == 0) {
                return "block_2_1.png";
            } else if (m_blockLevel == 1) {
                return "block_2_3.png";
            } else if (m_blockLevel == 2) {
                return "block_2_4.png";
            }
        case kBlockChick:
            if (m_blockLevel == 0) {
                return "block_3_1.png";
            } else if (m_blockLevel == 1) {
                return "block_3_3.png";
            } else if (m_blockLevel == 2) {
                return "block_3_4.png";
            }
        case kBlockHumanWhite:
            if (m_blockLevel == 0) {
                return "block_4_1.png";
            } else if (m_blockLevel == 1) {
                return "block_4_3.png";
            } else if (m_blockLevel == 2) {
                return "block_4_4.png";
            }
        case kBlockBearBrown:
            if (m_blockLevel == 0) {
                return "block_5_1.png";
            } else if (m_blockLevel == 1) {
                return "block_5_3.png";
            } else if (m_blockLevel == 2) {
                return "block_5_4.png";
            }
        case kBlockFlog:
            if (m_blockLevel == 0) {
                return "block_6_1.png";
            } else if (m_blockLevel == 1) {
                return "block_6_3.png";
            } else if (m_blockLevel == 2) {
                return "block_6_4.png";
            }
        case kBlockBearBlue:
            if (m_blockLevel == 0) {
                return "block_7_1.png";
            } else if (m_blockLevel == 1) {
                return "block_7_3.png";
            } else if (m_blockLevel == 2) {
                return "block_7_4.png";
            }

        default:
            CCAssert(false, "invalid blockType");
            return "";
    }
}

// 移動先の初期化
void BlockSprite::initNextPos()
{
    m_nextPosX = -1;
    m_nextPosY = -1;
}

// 移動先インデックスをセット
void BlockSprite::setNextPos(int nextPosX, int nextPosY)
{
    m_nextPosX = nextPosX;
    m_nextPosY = nextPosY;
}

// ブロックを移動する
void BlockSprite::moveBlock()
{
    if (m_postPositionIndex.x != -1 || m_postPositionIndex.y != -1) {
        CCPoint nowPosition = getBlockPosition(m_prePositionIndex.x, m_prePositionIndex.y);
        CCPoint nextPosition = getBlockPosition(m_postPositionIndex.x, m_postPositionIndex.y);

        // 次のポジションへ現在のポジションを更新
        m_positionIndex = m_postPositionIndex;

        // 消せるなら状態遷移
        if (isMakeChain()) {
        
            gameManager->allMoved = false;

            unschedule(schedule_selector(GameScene::showSwapChainPosition));

            // ヒントサークルが表示されていれば、消去する
            CCNode *circle = gameManager->m_background->getChildByTag(GameScene::kTagHintCircle);
            if(circle != NULL) {
                circle->removeFromParentAndCleanup(true);
            }
        }
        
        // 動かす前にタッチできないようにする
        m_isTouchFlag = false;
        
        CCMoveBy *move;
        if (m_blockState == kChanging || m_blockState == kRechanging) {
            move = CCMoveBy::create(SWAPPING_TIME, ccp(nextPosition.x - nowPosition.x, nextPosition.y - nowPosition.y));
        } else {
            move = CCMoveBy::create(MOVING_TIME, ccp(nextPosition.x - nowPosition.x, nextPosition.y - nowPosition.y));
        }
        this->runAction(move);
    }
}


// 登録されたパートナーの位置へブロックを動かす
void BlockSprite::changePosition()
{
    if (m_partnerBlock == NULL) {
    }

    if (m_blockState == kStopping) {
        // 状態遷移
        m_blockState = kChanging;
        // 過去のポジションを格納する
        m_prePositionIndex = m_positionIndex;
        // 次のポジションを格納する
        m_postPositionIndex = m_partnerBlock->m_positionIndex;
    } else if (m_blockState == kChanging) {
        // 状態遷移
        m_blockState = kRechanging;
        // 過去のポジションと入れ替える
        PositionIndex tmp = m_prePositionIndex;
        m_prePositionIndex = m_postPositionIndex;
        m_postPositionIndex = tmp;
    }
        
    // MOVING_TIME分のアニメーション移動を待つディレイタイム
    CCDelayTime *moveAnimationDelay = CCDelayTime::create(SWAPPING_TIME);
    
    // ブロックを動かす一連の処理
    CCCallFunc *func1 = CCCallFunc::create(this, callfunc_selector(BlockSprite::moveBlock));
    CCCallFunc *func2 = CCCallFunc::create(this, callfunc_selector(BlockSprite::changePositionFinished));
    CCFiniteTimeAction *action1 = CCSequence::create(func1, moveAnimationDelay, func2, NULL);
    
    // 新しいタグに更新しておく
    setTag(gameManager->getTag(m_postPositionIndex.x, m_postPositionIndex.y));
    
    runAction(action1);
}

void BlockSprite::changePositionFinished()
{
    gameManager->setDeletingFlags(gameManager->checkChain(this));
    
    // 入れ替え後、片方のブロックが消えた/変化した/落下予約状態になった場合
    if (m_blockState == kChanging && (m_partnerBlock->isMakeChain() || m_partnerBlock->m_blockState == kStopping || m_partnerBlock->m_blockState == kPreDropping)) {
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;

    // 再入れ替え前
    } else if (m_blockState == kChanging && (m_partnerBlock->m_blockState == kChanging || m_partnerBlock->m_blockState == kRechanging)) {
        changePosition();
        
    // チェインが存在した場合
    } else if (m_partnerBlock != NULL && m_blockState == kDeleting && m_partnerBlock->isMakeChain() && this != m_partnerBlock) {
        m_isTouchFlag = false;
        std::list<int> removeBlockTags = gameManager->checkChain(this);
        // 数によって消え方の設定を変える
        gameManager->searchAndSetDeleteType(removeBlockTags);
        
        if (!doubleDelete) {
            gameManager->removeBlocks(removeBlockTags);
            doubleDelete = true;
        } else {
            gameManager->removeBlocks(removeBlockTags);
            if (GameScene::addFlag) {
                while (!GameScene::addFlag){
                    
                }
                gameManager->addBlocks();

                gameManager->m_combo += 2;
                // 2コンボ以上のときはアニメ演出
                if (gameManager->m_combo >= 2) {
                    gameManager->showCombo();
                }
                
                gameManager->unschedule(schedule_selector(GameScene::resetCombo));
                gameManager->scheduleOnce(schedule_selector(GameScene::resetCombo), COMBO_TIME);
            } else {
                gameManager->addBlocks();

                gameManager->m_combo += 2;
                // 2コンボ以上のときはアニメ演出
                if (gameManager->m_combo >= 2) {
                    gameManager->showCombo();
                }
                
                gameManager->unschedule(schedule_selector(GameScene::resetCombo));
                gameManager->scheduleOnce(schedule_selector(GameScene::resetCombo), COMBO_TIME);
            }
        }
        
    } else if (m_blockState == kDeleting) {
        m_isTouchFlag = false;
        std::list<int> removeBlockTags = gameManager->checkChain(this);
        // 数によって消え方の設定を変える
        gameManager->searchAndSetDeleteType(removeBlockTags);

        if (3 <= removeBlockTags.size()) {
            gameManager->removeBlocks(removeBlockTags);
            if (GameScene::addFlag) {
                while (!GameScene::addFlag){
                    
                }
                
                gameManager->addBlocks();
                
                gameManager->m_combo++;
                // 2コンボ以上のときはアニメ演出
                if (gameManager->m_combo >= 2) {
                    gameManager->showCombo();
                }
                
                gameManager->unschedule(schedule_selector(GameScene::resetCombo));
                gameManager->scheduleOnce(schedule_selector(GameScene::resetCombo), COMBO_TIME);

            } else {
                gameManager->addBlocks();
                
                gameManager->m_combo++;
                // 2コンボ以上のときはアニメ演出
                if (gameManager->m_combo >= 2) {
                    gameManager->showCombo();
                }
                
                gameManager->unschedule(schedule_selector(GameScene::resetCombo));
                gameManager->scheduleOnce(schedule_selector(GameScene::resetCombo), COMBO_TIME);
            }
        }

    } else if (m_partnerBlock != NULL && m_partnerBlock->m_blockState == kDropping) {
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;
    // 再入れ替え終了後
        
    } else if (m_blockState == kRechanging) {
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;
    }
}

// 指定したブロックを含む３つ以上のブロック連結があるかどうか
bool BlockSprite::isMakeChain() {    
    // 横方向の繋がり
    int count = 1; // 横につながっている個数
    
    // 右方向に走査
    for (int x = m_positionIndex.x + 1 ; x < MAX_BLOCK_X; x++) {
        int targetTag = gameManager->getTag(x, m_positionIndex.y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    // 左方向に走査
    for (int x = m_positionIndex.x - 1; x >= 0; x--) {
        int targetTag = gameManager->getTag(x, m_positionIndex.y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    // 3つ繋がっているか
    if (3 <= count) {
        return true;
    }
    
    
    // 縦方向の繋がり
    count = 1; // 縦につながっている個数
    for (int y = m_positionIndex.y + 1; y < MAX_BLOCK_Y; y++) {
        int targetTag = gameManager->getTag(m_positionIndex.x, y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    for (int y = m_positionIndex.y - 1; y >= 0; y--) {
        int targetTag = gameManager->getTag(m_positionIndex.x, y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);              
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    // 3つ繋がっているか
    if (count >= 3) {
        return true;
    }
    
    return false;  // 3マッチがない
}

// 位置取得 (0 <= posIndexX <= 6 , 0 <= posIndexY <= 6)
CCPoint BlockSprite::getBlockPosition(int indexX, int indexY)
{
    return CCPoint((indexX + 0.5) * m_blockSizes, (indexY + 0.5) * m_blockSizes);
}

void BlockSprite::removeSelfAnimation()
{
    CCLog("tag = %d, level = %d", gameManager->getTag(m_positionIndex.x, m_positionIndex.y), m_blockLevel);
    if (m_blockLevel == 2) {
        CCLog("level2");
        m_blockLevel = 0;
        gameManager->lineDeleteAnimation(m_positionIndex.x, m_positionIndex.y);
        gameManager->linearDelete(m_positionIndex.x, m_positionIndex.y);
/*
    // レベル2のブロックが消えるとき、上下左右いっぱいのブロックを消す
        // 横方向
        for (int x = 0; x < MAX_BLOCK_X; x++) {
            int targetTag = gameManager->getTag(x, m_positionIndex.y);
            BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
            if (target == NULL || targetTag == cocos2d::CCNode::getTag() || target->m_blockState == kDeleting) {
                continue;
            }
            target->m_blockState = BlockSprite::kDeleting;
            target->removeSelfAnimation();
        }
        
        // 横方向
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            int targetTag = gameManager->getTag(m_positionIndex.x, y);
            BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
            if (target == NULL || targetTag == cocos2d::CCNode::getTag() || target->m_blockState == kDeleting) {
                continue;
            }
            target->m_blockState = BlockSprite::kDeleting;
            target->removeSelfAnimation();
        }
*/
    } else if (m_blockLevel == 1) {
        CCLog("b");
        m_blockLevel = 0;
        gameManager->aroundDelete(m_positionIndex.x, m_positionIndex.y);
/*
    // レベル1のブロックが消えるとき、周囲8方向のブロックを消す
        int tags[] = {
            1,      // 上
            -1,     // 下
            -100,   // 左
            100,    // 右
            -99,     // 左上
            -101,   // 左下
            101,    // 右上
            99,     // 右下
        };
        
        for (int i = 0; i < sizeof(tags) ; i++) {
            int targetTag = cocos2d::CCNode::getTag() + tags[i];
            BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
            if (target == NULL || targetTag == cocos2d::CCNode::getTag() || target->m_blockState == kDeleting) {
                continue;
            }
            target->m_blockState = BlockSprite::kDeleting;
            target->removeSelfAnimation();
        }
*/
    }
    
    if (m_isLevelUp) {
        m_isLevelUp = false;
        
        if (deleteState == kDeleteThree) {
        } else if (deleteState == kDeleteFour) {
            m_blockLevel = 1;
            setTexture(CCTextureCache::sharedTextureCache()->addImage(getBlockImageFileName(m_blockType)));
            m_blockState = kStopping;
            return;
            
        } else if (deleteState == kDeleteFive) {
            m_blockLevel = 2;
            setTexture(CCTextureCache::sharedTextureCache()->addImage(getBlockImageFileName(m_blockType)));
            m_blockState = kStopping;
            return;
        }
#pragma mark スコアアップ
    } else {
        if (deleteState == kDeleteThree) {
            gameManager->m_score += 500;
        } else if (deleteState == kDeleteFour) {
            gameManager->m_score += 1000;
        } else if (deleteState == kDeleteFive) {
            gameManager->m_score += 1500;
        }
    }
    changeImageToRemove();
    
    
    CCCallFunc *func1 = CCCallFunc::create(this, callfunc_selector(BlockSprite::removeSelf));
    CCCallFunc *func2 = CCCallFunc::create(this, callfunc_selector(BlockSprite::animExplosion));
    CCDelayTime *delay = CCDelayTime::create(REMOVING_TIME/2);
//    animExplosion(REMOVING_TIME);
    
    CCFiniteTimeAction *action = CCSequence::create(delay, func2, delay, func1, NULL);
    runAction(action);
    
    // 参照を消す
//    scheduleOnce(schedule_selector(BlockSprite::removeSelf), REMOVING_TIME);
}

void BlockSprite::changeImageToRemove() {
    switch (m_blockType) {
        case kBlockPig:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_1_2.png"));
            break;
        case kBlockHumanRed:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_2_2.png"));
            break;
        case kBlockChick:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_3_2.png"));
            break;
        case kBlockHumanWhite:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_4_2.png"));
            break;
        case kBlockBearBrown:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_5_2.png"));
            break;
        case kBlockFlog:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_6_2.png"));
            break;
        case kBlockBearBlue:
            setTexture(CCTextureCache::sharedTextureCache()->addImage("block_7_2.png"));
            break;
        default:
            break;
    }

}

void BlockSprite::removeSelf() {
#pragma mark その場に光の物体を出す処理
    if (collectSprite != NULL) {
#pragma mark powerSpriteの登録
        PowerUpSprite *powerSprite = PowerUpSprite::createPowerUpSprite("ui_star.png", m_positionIndex.x, m_positionIndex.y);
        powerSprite->toSprite = collectSprite;
        gameManager->addChild(powerSprite);
        gameManager->managePowerList.push_back(powerSprite);
    }
    
    removeFromParentAndCleanup(true);
}

void BlockSprite::dropBlock() {
    CCDelayTime *dropDelay = CCDelayTime::create(MOVING_TIME);
    CCCallFunc *func1 = CCCallFunc::create(this, callfunc_selector(BlockSprite::moveBlock));
    CCCallFunc *func2 = CCCallFunc::create(this, callfunc_selector(BlockSprite::dropFinished));
    
    // スワップ中ならmoveBlockの処理をしない.
    CCFiniteTimeAction *action;
    if (m_prePositionIndex.x != m_postPositionIndex.x) {
        action = CCSequence::create(dropDelay, func2, NULL);
    } else {
        m_blockState = kDropping;
        action = CCSequence::create(func1, dropDelay, func2, NULL);
    }

    runAction(action);
}

void BlockSprite::dropFinished() {
    m_isTouchFlag = true;
    m_blockState = kStopping;
    doubleDelete = false;

    gameManager->recursiveCheck();
}

void BlockSprite::animExplosion(float animTime = REMOVING_TIME)
{
    CCScaleTo* scale = CCScaleTo::create(animTime, 0);
    CCFadeOut* fadeOut = CCFadeOut::create(animTime);
    CCSpawn* explosion = CCSpawn::create(scale, fadeOut, NULL);
    
    CCSprite* effect = gameManager->getAnimation("normal");
    effect->setScale(0.7f);
    gameManager->addChild(effect);
    effect->setPosition(gameManager->getPosition(m_positionIndex.x, m_positionIndex.y));

    runAction(explosion);
}