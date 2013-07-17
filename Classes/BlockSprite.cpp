#include "BlockSprite.h"
#include "GameScene.h"

GameScene* BlockSprite::gameManager = NULL;
bool BlockSprite::doubleDelete = false;


BlockSprite::BlockSprite()
{
    initNextPos();
    m_swapPartnerTag = -1;
    m_blockState = kStopping;
    m_isTouchFlag = true;
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
        case kBlockRed:
            return "red.png";
            
        case kBlockBlue:
            return "blue.png";
            
        case kBlockYellow:
            return "yellow.png";

        case kBlockGreen:
            return "green.png";
        /*
        case kBlockGray:
            return "gray.png";
         
        case kBlockBlack:
            return "black.png";
        */
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
        

        CCLog("%d, %d", m_prePositionIndex.x, m_prePositionIndex.y);
        CCLog("%d, %d", m_postPositionIndex.x, m_postPositionIndex.y);
/*      CCLog("x = %f, y = %f", nowPosition.x, nowPosition.y);
        CCLog("xx = %f, y = %f", nextPosition.x, nextPosition.y);
*/
        // 次のポジションへ現在のポジションを更新
        m_positionIndex = m_postPositionIndex;

/*
        // 消せるなら状態遷移
        if (isMakeChain()) {
            CCLog("チェインあり");
            m_blockState = kDeleting;
        }
*/

        // 消せるなら状態遷移
        if (isMakeChain()) {
            CCLog("チェインあり");
            gameManager->setDeletingFlags(gameManager->checkChain(this));
//            gameManager->isChained = true;
            gameManager->allMoved = false;
            
            // ヒントサークルが表示されていれば、消去する
            CCNode *circle = gameManager->m_background->getChildByTag(GameScene::kTagHintCircle);
            if(circle != NULL) {
                circle->removeFromParentAndCleanup(true);
            }
        }
        
        
        // 動かす前にタッチできないようにする
        m_isTouchFlag = false;
        
        //CCMoveBy* move = CCMoveBy::create(MOVING_TIME, ccp(nextPosition.x - nowPosition.x, nextPosition.y - nowPosition.y));
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
        CCLog("Error : not setting partner block");
    }

    if (m_blockState == kStopping) {
        CCLog("changing");
        // 状態遷移
        m_blockState = kChanging;
        // 過去のポジションを格納する
        m_prePositionIndex = m_positionIndex;
        // 次のポジションを格納する
        m_postPositionIndex = m_partnerBlock->m_positionIndex;
    } else if (m_blockState == kChanging) {
        CCLog("Rechanging");
        // 状態遷移
        m_blockState = kRechanging;
        // 過去のポジションと入れ替える
        PositionIndex tmp = m_prePositionIndex;
        m_prePositionIndex = m_postPositionIndex;
        m_postPositionIndex = tmp;
    }
        
    // MOVING_TIME分のアニメーション移動を待つディレイタイム
    CCDelayTime *moveAnimationDelay = CCDelayTime::create(SWAPPING_TIME);
    
    CCLog("changeTime");
    // ブロックを動かす一連の処理
    CCCallFunc *func1 = CCCallFunc::create(this, callfunc_selector(BlockSprite::moveBlock));
    CCCallFunc *func2 = CCCallFunc::create(this, callfunc_selector(BlockSprite::changePositionFinished));
    CCFiniteTimeAction *action1 = CCSequence::create(func1, moveAnimationDelay, func2, NULL);
    
    // 新しいタグに更新しておく
    setTag(gameManager->getTag(m_postPositionIndex.x, m_postPositionIndex.y));
    
    runAction(action1);
}

void BlockSprite::changePositionFinished() {
    // 入れ替え後、片方のブロックが消えた場合
    if (m_blockState == kChanging && m_partnerBlock->m_blockState == kDeleting) {
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;
        CCLog("1");

    // 再入れ替え前
    } else if (m_blockState == kChanging && (m_partnerBlock->m_blockState == kChanging || m_partnerBlock->m_blockState == kRechanging)) {
        CCLog("2");
        changePosition();
    // チェインが存在した場合
    } else if (m_blockState == kDeleting && m_partnerBlock->m_blockState == kDeleting) {
        CCLog("ダブルデリート");
        m_partnerBlock = NULL;
        m_isTouchFlag = false;
        std::list<int> removeBlockTags = gameManager->checkChain(this);
        if (!doubleDelete) {
            gameManager->removeBlocks(removeBlockTags);
//            gameManager->removeChainBlocks();
            doubleDelete = true;
        } else {
            gameManager->removeBlocks(removeBlockTags);
            if (GameScene::addFlag) {
                while (!GameScene::addFlag){
                    
                }
                CCLog("3333");
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
//            gameManager->checkChain(this);
        }
        
    } else if (m_blockState == kDeleting) {
        CCLog("片方デリート");
        m_partnerBlock = NULL;
        m_isTouchFlag = false;
        std::list<int> removeBlockTags = gameManager->checkChain(this);
#pragma mark gameManager
        if (3 <= removeBlockTags.size()) {
            gameManager->removeBlocks(removeBlockTags);
            if (GameScene::addFlag) {
                while (!GameScene::addFlag){
                    
                }
                CCLog("3333");
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
//        gameManager->removeChainBlocks();
    } else if (m_partnerBlock->m_blockState == kDropping) {
        CCLog("パートナードロップ");
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;
    // 再入れ替え終了後
    } else if (m_blockState == kRechanging) {
        CCLog("3");
        m_isTouchFlag = true;
        m_partnerBlock = NULL;
        m_blockState = kStopping;
    }
}

// 指定したブロックを含む３つ以上のブロック連結があるかどうか
bool BlockSprite::isMakeChain() {    
    // 横方向の繋がり
    int count = 1; // 横につながっている個数
    
    //CCLog("m_blockType = %d", m_blockType);
    // 右方向に走査
    for (int x = m_positionIndex.x + 1 ; x < MAX_BLOCK_X; x++) {
        int targetTag = gameManager->getTag(x, m_positionIndex.y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    //CCLog("right = %d", count);
    
    // 左方向に走査
    for (int x = m_positionIndex.x - 1; x >= 0; x--) {
        int targetTag = gameManager->getTag(x, m_positionIndex.y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }

    //CCLog("left = %d", count);
    
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
    
    //CCLog("up = %d", count);
    
    for (int y = m_positionIndex.y - 1; y >= 0; y--) {
        int targetTag = gameManager->getTag(m_positionIndex.x, y);
        BlockSprite *target = (BlockSprite *)gameManager->m_background->getChildByTag(targetTag);              
        if (target == NULL || target->getBlockType() != m_blockType) {
            break;
        }
        count++;
    }
    
    //CCLog("down = %d", count);
    
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
    /*
    // ブロックが消えるアニメーションを生成
    CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0);

    // REMOVING_TIME分のアニメーションを待つディレイタイム
    CCDelayTime *animationDelay = CCDelayTime::create(REMOVING_TIME);
    
    // 参照を消す
    CCCallFunc *func = CCCallFunc::create(this, callfunc_selector(BlockSprite::removeSelf));
    
    // 消すアニメーションの後に、参照を削除
    CCFiniteTimeAction* action = CCSequence::create(scale, animationDelay, func, NULL);
     
     // アクションを実行
     runAction(action);
    */
    
    animExplosion(REMOVING_TIME);
    //animSplash(REMOVING_TIME);
    
    // 参照を消す
    scheduleOnce(schedule_selector(BlockSprite::removeSelf), REMOVING_TIME);
}

void BlockSprite::removeSelf() {
    CCLog("removeSelf");
    removeFromParentAndCleanup(true);
}

void BlockSprite::dropBlock() {
    CCDelayTime *dropDelay = CCDelayTime::create(MOVING_TIME);
    CCCallFunc *func1 = CCCallFunc::create(this, callfunc_selector(BlockSprite::moveBlock));
    CCCallFunc *func2 = CCCallFunc::create(this, callfunc_selector(BlockSprite::dropFinished));
    
    CCLog("pre.x = %d, pre.y = %d", m_prePositionIndex.x, m_prePositionIndex.y);
    CCLog("post.x = %d, post.y = %d", m_postPositionIndex.x, m_postPositionIndex.y);
    CCLog("newTag = %d",gameManager->getTag(m_postPositionIndex.x, m_postPositionIndex.y));
    CCLog("blockState = %d", m_blockState);
    
    // スワップ中ならmoveBlockの処理をしない.
    CCFiniteTimeAction *action;
    if (m_prePositionIndex.x != m_postPositionIndex.x) {
        CCLog("省略");
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

    /*
    CCDelayTime *delayTime = CCDelayTime::create(REMOVING_TIME * 2.5f);
    CCCallFunc *recursiveFunc = CCCallFunc::create(gameManager, callfunc_selector(GameScene::recursiveCheck));
    CCFiniteTimeAction *action = CCSequence::create(delayTime, recursiveFunc, NULL);
    runAction(action);
    */
    
    CCLog("myTag = %d", gameManager->getTag(m_positionIndex.x, m_positionIndex.y));
    gameManager->recursiveCheck();
}

void BlockSprite::animExplosion(float animTime)
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

void BlockSprite::animSplash(float animTime)
{
    CCScaleTo* scale = CCScaleTo::create(animTime, 0);

    CCSprite* waters[5];
    for (int i = 0; i < sizeof(waters); i++) {
        waters[i] = CCSprite::create("water.png");
        waters[i]->setPosition(gameManager->getPosition(m_positionIndex.x, m_positionIndex.y));
        
        int vecX = rand() % 20 - 10;
        int jumpHeight = rand() % 15 + 5;
        CCJumpBy* actionJump = CCJumpBy::create(0.8f, ccp(vecX, 0), jumpHeight, 1);
        waters[i]->runAction(actionJump);
        
        gameManager->m_background->addChild(waters[i]);
    }
    
}

void BlockSprite::animJump(float animTime)
{
    
}

void BlockSprite::animFloat(float animTime)
{
    
}

void BlockSprite::animHopSplash(float animTime)
{
    
}

void BlockSprite::animDrop(float animTime)
{
    
}

void BlockSprite::animLowJumpAndDrop(float animTime)
{
    
}

void BlockSprite::animSlash(float animTime)
{
    
}

void BlockSprite::animImage(float animTime)
{
    
}

void BlockSprite::animPress(float animTime)
{
    
}
