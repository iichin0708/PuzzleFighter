 #include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "CCPlaySE.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

//初期化
int GameScene::preTouchTag = -1;
int GameScene::postTouchTag = -1;
list<int> GameScene::removeBlockTagLists;

CCScene* GameScene::scene()
{
    CCScene* scene = CCScene::create();
    GameScene* layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

// 初期化
bool GameScene::init()
{
    if (!CCLayer::init()) { return false; }
    
    // タップイベントを取得する
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    
    // バックキー・メニューキーイベントを取得する
    setKeypadEnabled(true);
    
    // 変数初期化
    initForVariables();
    
    // 背景表示
    showBackground();
    
    // ブロック表示
    showBlock();
    
    // リセットボタン作成
    showResetButton();
    
    // 効果音の事前読み込み
    SimpleAudioEngine::sharedEngine()->preloadEffect(MP3_REMOVE_BLOCK);
    
    return true;
}


// 変数初期化
void GameScene::initForVariables()
{
    // 乱数初期化
    srand((unsigned)time(NULL));
    
    // ブロックの一辺の長さを取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;
    
    // 変数初期化
    m_animating = false;
    m_score = 0;
}


// 背景表示
void GameScene::showBackground()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // 背景を生成
    m_background = CCSprite::create(PNG_BACKGROUND);
    m_background->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    addChild(m_background, kZOrderBackground, kTagBackground);
}


// ブロック表示
void GameScene::showBlock()
{
    do {
        // 6 x 6 のブロックを作成する
        for (int x = 0; x < MAX_BLOCK_X; x++)
        {
            for (int y = 0; y < MAX_BLOCK_Y; y++)
            {
                // 設置ブロックのタイプの除外リスト
                list<int> matchTypes;
                matchTypes.clear();
                
                // 横の後ろ2つをチェック
                int blockTag1 = kTagBaseBlock + (x - 1) * 100 + y;
                BlockSprite *block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
                int blockTag2 = kTagBaseBlock + (x - 2) * 100 + y;
                BlockSprite *block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
                
                // 2つ同じブロックが並んでいれば、設置ブロックのタイプの除外リストに追加
                if (block1 != NULL &&
                    block2 != NULL &&
                    block1->getBlockType() == block2->getBlockType())
                {
                    matchTypes.push_back(block1->getBlockType());
                }
                
                // 横の後ろ2つをチェック
                blockTag1 = kTagBaseBlock + x * 100 + y - 1;
                block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
                blockTag2 = kTagBaseBlock + x * 100 + y - 2;
                block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
                
                // 2つ同じブロックが並んでいれば、設置ブロックのタイプの除外リストに追加
                if (block1 != NULL &&
                    block2 != NULL &&
                    block1->getBlockType() == block2->getBlockType())
                {
                    matchTypes.push_back(block1->getBlockType());
                }
                
                bool isMatch = true;
                
                kBlock blockType;
                while (isMatch) {
                    isMatch = false;
                    
                    // ランダムでブロックを作成
                    blockType = (kBlock)(rand() % kBlockCount);
                    
                    // 除外リストと比較して、一致したら生成し直し
                    list<int>::iterator it = matchTypes.begin();
                    while( it != matchTypes.end() ) {
                        if(blockType == *it) {
                            isMatch = true;
                        }
                        ++it;
                    }
                }
                
                // 対応するブロック配列にタグを追加
                int tag = getTag(x, y);
                
                // ブロックを作成
                BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
                pBlock->setPosition(getPosition(x, y));
                m_background->addChild(pBlock, kZOrderBlock, tag);
            }
        }
    } while (getSwapChainCount() <= 0);
}


// 位置取得 (0 <= posIndexX <= 6 , 0 <= posIndexY <= 6)
CCPoint GameScene::getPosition(int posIndexX, int posIndexY)
{
    float offsetX = m_background->getContentSize().width * 0.168 + DISP_POSITION_X;
    float offsetY = m_background->getContentSize().height * 0.029 + DISP_POSITION_Y;
    return CCPoint((posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY);
}

// タグ取得 (0 <= posIndexX <= 6 , 0 <= posIndexY <= 6)
int GameScene::getTag(int posIndexX, int posIndexY)
{
    return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

// タッチ開始イベント
bool GameScene::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    
    // アニメーション中はタップ処理を受け付けない
    if(!m_animating) {
        preTouchTag = -1;
        CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
        int tag = 0;
        kBlock blockType;
        getTouchBlockTag(touchPoint, tag, blockType);
        //触った場所にブロックがあった場合
        if (tag != 0) {
            preTouchTag = tag;
            return true;
        }
    }
    return false;
}

// タッチ移動イベント
void GameScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);

    if (tag != 0 && !m_animating) {
        postTouchTag = tag;
        
        if (checkCorrectSwap(preTouchTag, postTouchTag)) {
            // コンボの初期化
            m_combo = 0;
            swapSprite();
            scheduleOnce(schedule_selector(GameScene::checkAndRemoveAndDrop), MOVING_TIME);
        }
    }
}

// タッチ終了イベント
void GameScene::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}

//上下左右に動いたかどうか(正しいスワップがされたかどうか)
bool GameScene::checkCorrectSwap(int preTag, int postTag)
{
    int tags[] = {
        preTouchTag + 100,
        preTouchTag - 100,
        preTouchTag + 1,
        preTouchTag - 1,
    };
    
    for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
        if (tags[i] == postTag) {
            // アニメーション開始
            m_animating = true;
            return true;
        }
    }
    
    return false;
}

// 2つのスプライトを入れ替える
void GameScene::swapSprite()
{
    //入れ替わるアニメーションを挿入
    BlockSprite *preTouchSprite = (BlockSprite *)m_background->getChildByTag(preTouchTag);
    BlockSprite *postTouchSprite = (BlockSprite *)m_background->getChildByTag(postTouchTag);
    
    PositionIndex prePositionIndex = getPositionIndex(preTouchTag);
    PositionIndex postPositionIndex = getPositionIndex(postTouchTag);
    
    CCMoveTo *preTouchSpriteMove = CCMoveTo::create(MOVING_TIME, getPosition(postPositionIndex.x, postPositionIndex.y));
    CCMoveTo *postTouchSpriteMove = CCMoveTo::create(MOVING_TIME, getPosition(prePositionIndex.x, prePositionIndex.y));
    
    preTouchSprite->runAction(preTouchSpriteMove);
    postTouchSprite->runAction(postTouchSpriteMove);
    
    //タグの入れ替え
    preTouchSprite->setTag(postTouchTag);
    postTouchSprite->setTag(preTouchTag);
    
}

// 削除できるブロックがあれば、removeAndDropを呼び出す
void GameScene::checkAndRemoveAndDrop()
{
    list<int> removeBlockTags = getRemoveChainBlocks();
    
    // 消えることのできるブロックがある
    if(removeBlockTags.size() >= 3) {
        m_combo++;
        
        // 2コンボ以上のときはアニメ演出
        if (m_combo >= 2) {
            CCLOG("%d combo!", m_combo);
            char comboText[10];
            sprintf(comboText, "%d COMBO!", m_combo);
            CCLabelTTF *comboLabel = CCLabelTTF::create(comboText, "arial", 60);
            
            // 表示できる画面サイズ取得
            CCDirector* pDirector = CCDirector::sharedDirector();
            CCPoint origin = pDirector->getVisibleOrigin();
            CCSize visibleSize = pDirector->getVisibleSize();
            
            comboLabel->setPosition(ccp(origin.x + visibleSize.width / 2,
                                        origin.y + visibleSize.height / 2));
            comboLabel->setColor(ccc3(0, 0, 0));
            addChild(comboLabel);
            
            float during = 0.5f;
            CCFadeOut *actionFadeOut = CCFadeOut::create(during);
            CCScaleTo *actionScaleUp = CCScaleTo::create(during, 1.5f);
            comboLabel->runAction(actionFadeOut);
            comboLabel->runAction(actionScaleUp);
            
            comboLabel->scheduleOnce(schedule_selector(CCLabelTTF::removeFromParent), during);
        }
        
        removeBlockTagLists = removeBlockTags;
        
        // 得点加算 (消したブロック数 - 2) の2 乗
        m_score += pow(removeBlockTags.size() - 2, 2);
        
        removeBlocksAniamtion(removeBlockTags, REMOVING_TIME);
        
        scheduleOnce(schedule_selector(GameScene::removeAndDrop), REMOVING_TIME);
    } else {
        // ブロック入れ替え直後であれば、元に戻したあと入力受付状態にする
        if(preTouchTag != -1 && postTouchTag != -1) {
            swapSprite();
            scheduleOnce(schedule_selector(GameScene::exchangeAnimationFinished), MOVING_TIME);
        } else {
            m_animating = false;
            // CCLOG("潜在連結数 : %d", getSwapChainCount());
            // 潜在的な連結がないとき
            if (getSwapChainCount() <= 0) {
                // TODO:盤面を新しく用意する
                CCLOG("No Match!");
            }
        }
    }
}

// 入れ替えアニメーションの終了
void GameScene::exchangeAnimationFinished() {
    m_animating = false;
}

// 連結していて消滅できるブロックの、タグ配列を取得
list<int> GameScene::getRemoveChainBlocks()
{
    // 消滅できるブロックリスト
    list<int> removeChainBlocks;
    
    if(preTouchTag != -1 && postTouchTag != -1) {
        // 移動させたブロックが連結になったか
        if (! isChainedBlock(preTouchTag) &&
            ! isChainedBlock(postTouchTag))
        {
            // 連結がなければ消えるブロックなし
            return removeChainBlocks;
        }
    }
    
    // タッチしたブロックのタグを初期化
    preTouchTag = -1;
    postTouchTag = -1;
    
    // 消滅候補ブロックリスト
    list<int> removeReserveBlocks;
    
    // 1行ずつ横の連なりを走査
    for (int y = 0; y <= 5; y++) {
        // 比較対象のブロックの種類
        kBlock currentType;
        
        for (int x = 0; x <= 5; x++) {
            // ターゲットのブロックを取得
            int targetTag = kTagBaseBlock + x * 100 + y;
            BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
            kBlock targetType = target->getBlockType();
            
            // カレントとターゲットが同じ種類のブロックかどうか
            if (targetType == currentType) {
                // 同じなら消滅候補に追加
                removeReserveBlocks.push_back(targetTag);
                
            } else {
                // 違うならカレントをターゲットに変更
                currentType = targetType;
                
                // その時点で消滅候補が３つ以上（繋がりが３つ以上）なら
                if (removeReserveBlocks.size() >= 3) {
                    list<int>::iterator it = removeReserveBlocks.begin(); // イテレータ
                    while( it != removeReserveBlocks.end() ) {
                        removeChainBlocks.push_back(*it);
                        ++it;  // イテレータを１つ進める
                    }
                }
                
                // 消滅候補を空にして、ターゲットを追加
                removeReserveBlocks.clear();
                removeReserveBlocks.push_back(targetTag);
            }
        }
        
        // 対象の行の走査終了時も消滅候補をチェックして空にする
        if (removeReserveBlocks.size() >= 3) {
            list<int>::iterator it = removeReserveBlocks.begin(); // イテレータ
            while( it != removeReserveBlocks.end() ) {
                removeChainBlocks.push_back(*it);
                ++it;  // イテレータを１つ進める
            }
        }
        
        removeReserveBlocks.clear();
    }
    
    
    // 1列ずつ縦の連なりを走査
    for (int x = 0; x <= 5; x++) {
        // 比較対象のブロックの種類
        kBlock currentType;
        
        for (int y = 0; y <= 5; y++) {
            // ターゲットのブロックを取得
            int targetTag = kTagBaseBlock + x * 100 + y;
            BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
            kBlock targetType = target->getBlockType();
            
            // カレントとターゲットが同じ種類のブロックかどうか
            if (targetType == currentType) {
                // 同じなら消滅候補に追加
                removeReserveBlocks.push_back(targetTag);
                
            } else {
                // 違うならカレントをターゲットに変更
                currentType = targetType;
                
                // その時点で消滅候補が３つ以上（繋がりが３つ以上）なら
                if (removeReserveBlocks.size() >= 3) {
                    list<int>::iterator it = removeReserveBlocks.begin(); // イテレータ
                    while( it != removeReserveBlocks.end() ) {
                        removeChainBlocks.push_back(*it);
                        ++it;  // イテレータを１つ進める
                    }
                }
                
                // 消滅候補を空にして、ターゲットを追加
                removeReserveBlocks.clear();
                removeReserveBlocks.push_back(targetTag);
            }
        }
        
        // 対象の行の走査終了時も消滅候補をチェックして空にする
        if (removeReserveBlocks.size() >= 3) {
            list<int>::iterator it = removeReserveBlocks.begin(); // イテレータ
            while( it != removeReserveBlocks.end() ) {
                removeChainBlocks.push_back(*it);
                ++it;  // イテレータを１つ進める
            }
        }
        
    }
    
    removeChainBlocks.sort();
    
    removeChainBlocks.unique();
    
    return removeChainBlocks;
}


// 指定したブロックを含む３つ以上のブロック連結があるかどうか
bool GameScene::isChainedBlock(int blockTag)
{
    BlockSprite *block = (BlockSprite *)m_background->getChildByTag(blockTag);
    
    // ブロックの種類
    kBlock blockType = block->getBlockType();
    // ブロックの盤面上の座標
    PositionIndex blockIndex = getPositionIndex(blockTag);
    
    // 横方向の繋がり
    int count = 1; // 横につながっている個数
    // 右方向に走査
    for (int x = blockIndex.x + 1 ; x < MAX_BLOCK_X; x++) {
        int targetTag = kTagBaseBlock + x * 100 + blockIndex.y;
        BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != blockType) {
            break;
        }
        count++;
    }
    
    // 左方向に走査
    for (int x = blockIndex.x - 1; x >= 0; x--) {
        int targetTag = kTagBaseBlock + x * 100 + blockIndex.y;
        BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != blockType) {
            break;
        }
        count++;
    }
    // 3つ繋がっているか
    if (count >= 3) { return true; }
    
    
    // 縦方向の繋がり
    count = 1; // 縦につながっている個数
    for (int y = blockIndex.y + 1; y < MAX_BLOCK_Y; y++) {
        int targetTag = kTagBaseBlock + blockIndex.x * 100 + y;
        BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != blockType) {
            break;
        }
        count++;
    }
    
    for (int y = blockIndex.y - 1; y >= 0; y--) {
        int targetTag = kTagBaseBlock + blockIndex.x * 100 + y;
        BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
        if (target == NULL || target->getBlockType() != blockType) {
            break;
        }
        count++;
    }
    // 3つ繋がっているか
    if (count >= 3) { return true; }
    
    return false;  // 3マッチがない
}

// 配列のブロックの消えるアニメーションを実行
void GameScene::removeBlocksAniamtion(list<int> blockTags, float during)
{
    bool first = true;
    
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end())
    {
        // 対象となるブロックを取得
        CCNode* block = m_background->getChildByTag(*it);
        if (block)
        {
            // ブロックが消えるアニメーションを生成
            CCScaleTo* scale = CCScaleTo::create(during, 0);
            
            CCFiniteTimeAction* action;
            if (first)
            {
                // ブロックが消えるサウンドアクションを生成
                CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);
                
                // アクションをつなげる
                action = CCSpawn::create(scale, playSe, NULL);
                
                first = false;
            }
            else
            {
                action = scale;
            }
            
            // アクションをセットする
            block->runAction(action);
        }
        
        it++;
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}


// 消滅リスト内のブロックを消して、上のブロックを落とすアニメーションセット
void GameScene::removeAndDrop()
{
    // 隣接するブロックを削除する
    removeBlock(removeBlockTagLists);
    

    // ブロック削除後のアニメーション
    movingBlocksAnimation(removeBlockTagLists);
    
    removeBlockTagLists.clear();
}

// 配列のブロックを削除
void GameScene::removeBlock(list<int> blockTags)
{
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end())
    {
        // 対象となるブロックを取得
        CCNode* block = m_background->getChildByTag(*it);
        if (block)
        {
            // コマの削除
            block->removeFromParentAndCleanup(true);
        }
        it++;
    }
}

// ブロック削除後のアニメーション
void GameScene::movingBlocksAnimation(list<int> blocks)
{
    // 削除された場所に既存のピースをずらす
    searchNewPosition(blocks);
    int spriteCount = 0;
    spriteCount = 0;
    
    
    // 新しい位置がセットされたピースのアニメーション
    moveBlock();
    
    // 新しいブロックを場外に追加
    dropNewBlocks();
    
    // 場外から落とす
    moveBlock();
    
    scheduleOnce(schedule_selector(GameScene::dropAnimationFinished), MOVING_TIME * 2);
}

// 消えたブロックを埋めるように新しい位置をセット
void GameScene::searchNewPosition(list<int> blocks)
{
    // 消えるブロック数分のループ
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end())
    {
        PositionIndex posIndex1 = getPositionIndex(*it1);
        
        //消えるブロックより上にあるブロックを下にずらす(ポジションのセット)
        for (int x = 0; x < MAX_BLOCK_X; x++) {
            for (int y = 0; y < MAX_BLOCK_Y; y++) {
                if (posIndex1.x == x && posIndex1.y < y) {
                    int tag = getTag(x, y);
                    BlockSprite *blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
                    if(blockSprite != NULL) {
                        PositionIndex pos = getPositionIndex(tag);
                        setNewPosition(tag, pos);
                    }
                }
            }
        }
        it1++;
    }
}

// 新しい位置をセット
void GameScene::setNewPosition(int tag, PositionIndex posIndex)
{
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    
    
    int nextPosY = blockSprite->getNextPosY();
    if (nextPosY == -1)
    {
        nextPosY = posIndex.y;
    }
    
    // 移動先の位置をセット
    blockSprite->setNextPos(posIndex.x, --nextPosY);
}


// ブロックを移動する
void GameScene::moveBlock()
{
    for(int x = 0; x < MAX_BLOCK_X; x++) {
        for( int y = 0; y < MAX_BLOCK_Y; y++) {
            int tag = getTag(x, y);
            BlockSprite *blockSprite = (BlockSprite *)m_background->getChildByTag(tag);
            if(blockSprite != NULL) {
                int nextPosX = blockSprite->getNextPosX();
                int nextPosY = blockSprite->getNextPosY();
                
                if(nextPosX != -1 || nextPosY != -1) {
                    int newTag = getTag(nextPosX, nextPosY);
                    blockSprite->initNextPos();
                    blockSprite->setTag(newTag);
                    
                    CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                    blockSprite->runAction(move);
                }
            }
        }
    }
    
}

//新しいブロックを落とす
void GameScene::dropNewBlocks()
{
    for (int x = 0; x < MAX_BLOCK_X; x++) {
        int removedCount = 0;
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            int tag = getTag(x, y);
            BlockSprite *bSprite = (BlockSprite *)m_background->getChildByTag(tag);
            //消えたスプライトの数を取得
            if (bSprite == NULL) {
                removedCount++;
            }
        }
        
        //追加
        for (int i = 0; 0 < removedCount; removedCount--, i++) {
            kBlock blockType = (kBlock)(rand() % kBlockCount);
            
            //列の空きの中でも上から順番にタグをセットする.
            int tag = getTag(x, MAX_BLOCK_Y - removedCount);
            
            BlockSprite *pBlock = BlockSprite::createWithBlockType(blockType);
            
            //画面外に配置
            pBlock->setPosition(getPosition(x, MAX_BLOCK_Y + i));
            //落ちる目的地はタグの場所
            pBlock->setNextPos(x, MAX_BLOCK_Y - removedCount);
            m_background->addChild(pBlock, kZOrderBlock, tag);
            
        }
        
    }
    
}

// ブロックの移動完了
void GameScene::dropAnimationFinished()
{
    // 続けて連結があるかチェックして、消す
    // 消せなければアニメーション終了
    checkAndRemoveAndDrop();
}

// ブロックのインデックス取得
GameScene::PositionIndex GameScene::getPositionIndex(int tag)
{
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;
    
    return PositionIndex(pos1_x, pos1_y);
}

// 指定したブロックに潜在的な連結があるかどうか
int GameScene::getSwapChainBlockCount(int blockTag)
{
    int chainCount = 0;

    BlockSprite *block = (BlockSprite *)m_background->getChildByTag(blockTag);
    
    // ブロックの種類
    kBlock blockType = block->getBlockType();
    // ブロックの盤面上の座標
    int x = getPositionIndex(blockTag).x;
    int y = getPositionIndex(blockTag).y;
    
    // 間を1つ空けて横の後ろ2つをチェック
    int blockTag1 = kTagBaseBlock + (x - 2) * 100 + y;
    BlockSprite *block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
    int blockTag2 = kTagBaseBlock + (x - 3) * 100 + y;
    BlockSprite *block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
    
    // 一つ空けてターゲットのブロックと同じブロックが二つ並んでいたら
    // 潜在的なブロックと見なす
    if (block1 != NULL &&
        block2 != NULL &&
        block1->getBlockType() == block2->getBlockType() &&
        blockType == block1->getBlockType())
    {
        chainCount++;
    }
    
    // 間を1つ空けて前の後ろ2つをチェック
    blockTag1 = kTagBaseBlock + (x + 2) * 100 + y;
    block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
    blockTag2 = kTagBaseBlock + (x + 3) * 100 + y;
    block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
    
    // 一つ空けてターゲットのブロックと同じブロックが二つ並んでいたら
    // 潜在的なブロックと見なす
    if (block1 != NULL &&
        block2 != NULL &&
        block1->getBlockType() == block2->getBlockType() &&
        blockType == block1->getBlockType())
    {
        chainCount++;
    }
    
    // 間を1つ空けて縦の後ろ2つをチェック
    blockTag1 = kTagBaseBlock + x * 100 + y - 2;
    block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
    blockTag2 = kTagBaseBlock + x * 100 + y - 3;
    block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
    
    // 一つ空けてターゲットのブロックと同じブロックが二つ並んでいたら
    // 潜在的なブロックと見なす
    if (block1 != NULL &&
        block2 != NULL &&
        block1->getBlockType() == block2->getBlockType() &&
        blockType == block1->getBlockType())
    {
        chainCount++;
    }
    
    // 間を1つ空けて縦の前2つをチェック
    blockTag1 = kTagBaseBlock + x * 100 + y + 2;
    block1 = (BlockSprite *)m_background->getChildByTag(blockTag1);
    blockTag2 = kTagBaseBlock + x * 100 + y + 3;
    block2 = (BlockSprite *)m_background->getChildByTag(blockTag2);
    
    // 一つ空けてターゲットのブロックと同じブロックが二つ並んでいたら
    // 潜在的なブロックと見なす
    if (block1 != NULL &&
        block2 != NULL &&
        block1->getBlockType() == block2->getBlockType() &&
        blockType == block1->getBlockType())
    {
        chainCount++;
    }
    
    int tags[] = {
        1,      // 上
        -1,     // 下
        -100,   // 左
        100     // 右
    };
    
    for (int i = 0; i < sizeof(tags) ; i++) {
        int nextToBlockTag = blockTag + tags[i];
        PositionIndex nextToBlockIndex = getPositionIndex(nextToBlockTag);

        // 縦の走査のとき
        if (tags[i] == 1 || tags[i] == -1) {
            // 横方向の繋がり
            int count = 1; // 横につながっている個数
            // 右方向に走査
            for (int tx = nextToBlockIndex.x + 1; tx <= nextToBlockIndex.x + 2; tx++) {
                int targetTag = kTagBaseBlock + tx * 100 + nextToBlockIndex.y;
                BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
                if (target == NULL || target->getBlockType() != blockType) {
                    break;
                }
                if (targetTag != blockTag) {
                    count++;
                }
            }
            
            // 左方向に走査
            for (int tx = nextToBlockIndex.x - 1; tx >= nextToBlockIndex.x - 2; tx--) {
                int targetTag = kTagBaseBlock + tx * 100 + nextToBlockIndex.y;
                BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
                if (target == NULL || target->getBlockType() != blockType) {
                    break;
                }
                if (targetTag != blockTag) {
                    count++;
                }
            }
            
            // 3つ繋がっているか
            if (count >= 3) { chainCount++; }
        }

        
        // 横の走査のとき
        if (tags[i] == 100 || tags[i] == -100) {
            // 縦方向の繋がり
            int count = 1; // 縦につながっている個数
            for (int ty = nextToBlockIndex.y + 1; ty <= nextToBlockIndex.y + 2; ty++) {
                int targetTag = kTagBaseBlock + nextToBlockIndex.x * 100 + ty;
                BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
                if (target == NULL || target->getBlockType() != blockType) {
                    break;
                }
                if (targetTag != blockTag) {
                    count++;
                }
            }
            
            for (int ty = nextToBlockIndex.y - 1; ty >= nextToBlockIndex.y - 2; ty--) {
                int targetTag = kTagBaseBlock + nextToBlockIndex.x * 100 + ty;
                BlockSprite *target = (BlockSprite *)m_background->getChildByTag(targetTag);
                if (target == NULL || target->getBlockType() != blockType) {
                    break;
                }
                if (targetTag != blockTag) {
                    count++;
                }
            }
            
            // 3つ繋がっているか
            if (count >= 3) { chainCount++; }
        }
    }

    
    return chainCount;
}

// 潜在的な連結の数を取得する
int GameScene::getSwapChainCount()
{
    int chainCount = 0;
    
    for (int x = 0; x < MAX_BLOCK_X; x++) {
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            int blockTag = kTagBaseBlock + x * 100 + y;
            chainCount += getSwapChainBlockCount(blockTag);
        }
    }
    
    return chainCount;
}

// タップされたブロックのタグを取得
void GameScene::getTouchBlockTag(CCPoint touchPoint, int &tag, kBlock &blockType)
{
    for (int x = 0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++)
        {
            int currentTag = getTag(x, y);
            CCNode* node = m_background->getChildByTag(currentTag);
            if (node && node->boundingBox().containsPoint(touchPoint))
            {
                tag = currentTag;
                blockType = ((BlockSprite*)node)->getBlockType();
                
                return;
            }
        }
    }
}

// リセットボタンタップ時の処理
void GameScene::menuResetCallback(cocos2d::CCObject* pSender)
{
    GameScene* scene = GameScene::create();
    CCDirector::sharedDirector()->replaceScene((CCScene*)scene);
}

// リセットボタン作成
void GameScene::showResetButton()
{
    CCSize bgSize = m_background->getContentSize();
    
    // リセットボタン作成
    CCMenuItemImage* resetButton = CCMenuItemImage::create(PNG_RESET, PNG_RESET, this, menu_selector(GameScene::menuResetCallback));
    resetButton->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.1));
    
    // メニュー作成
    CCMenu* menu = CCMenu::create(resetButton, NULL);
    menu->setPosition(CCPointZero);
    m_background->addChild(menu);
}

// Androidバックキーイベント
void GameScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}

// Androidメニューキーイベント
void GameScene::keyMenuClicked()
{
    menuResetCallback(NULL);
}
