#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "CCPlaySE.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

int GameScene::preTouchTag = -1;
int GameScene::postTouchTag = -1;

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
    if (!CCLayer::init())
    {
        return false;
    }
    
    // タップイベントを取得する
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    
    // バックキー・メニューキーイベントを取得する
    setKeypadEnabled(true);
    
    // 変数初期化
    initForVariables();
    
    // 背景表示
    showBackground();
    
    // コマ表示
    showBlock();
    
    // ラベル作成
    showLabel();
    showHighScoreLabel();
    
    // リセットボタン作成
    showResetButton();
    
    // 効果音の事前読み込み
    SimpleAudioEngine::sharedEngine()->preloadEffect(MP3_REMOVE_BLOCK);
    
    return true;
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

// 変数初期化
void GameScene::initForVariables()
{
    // 乱数初期化
    srand((unsigned)time(NULL));
    
    // コマの一辺の長さを取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;

    // コマ種類の配列生成
    blockTypes.push_back(kBlockRed);
    blockTypes.push_back(kBlockBlue);
    blockTypes.push_back(kBlockYellow);
    blockTypes.push_back(kBlockGreen);
    blockTypes.push_back(kBlockGray);

    // 変数初期化
    m_animating = false;
    m_score = 0;
}

// 位置取得 (0 <= posIndexX <= 6 , 0 <= posIndexY <= 6)
CCPoint GameScene::getPosition(int posIndexX, int posIndexY)
{
    float offsetX = m_background->getContentSize().width * 0.168;
    float offsetY = m_background->getContentSize().height * 0.029;
    return CCPoint((posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY);
}

// タグ取得 (0 <= posIndexX <= 6 , 0 <= posIndexY <= 6)
int GameScene::getTag(int posIndexX, int posIndexY)
{
    return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

// コマ表示
void GameScene::showBlock()
{
    // 6 x 6 のコマを作成する
    for (int x = 0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++)
        {
            // ランダムでコマを作成
            kBlock blockType = (kBlock)(rand() % kBlockCount);
            
            // 対応するコマ配列にタグを追加
            int tag = getTag(x, y);
            m_blockTags[blockType].push_back(tag);
            
            // コマを作成
            BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
            pBlock->setPosition(getPosition(x, y));
            m_background->addChild(pBlock, kZOrderBlock, tag);
        }
    }
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

void GameScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);
    if (tag != 0) {
        postTouchTag = tag;
        if (checkCorrectSwap(preTouchTag, postTouchTag)) {
            swapSprite();            
            preTouchTag = -1;
            postTouchTag = -1;
        }
    }
       
}

// タッチ終了イベント
void GameScene::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    // タップポイント取得
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);
    
    // タップしたコマのTag とコマの種類を取得
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);
 
    if (tag != 0)
    {
        // 隣接するコマを検索する
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);
        
        if (sameColorBlockTags.size() > 1)
        {
            // 得点加算 (消したコマ数 - 2) の2 乗
            m_score += pow(sameColorBlockTags.size() - 2, 2);
            
            // アニメーション開始
            m_animating = true;
            
            // 隣接するコマを削除する
            removeBlock(sameColorBlockTags, blockType);
            
            // コマ削除後のアニメーション
            movingBlocksAnimation1(sameColorBlockTags);
            

        }
    }

}

//上下左右に動いたかどうか
bool GameScene::checkCorrectSwap(int preTag, int postTag) {
    int tags[] = {
        preTouchTag + 100,
        preTouchTag - 100,
        preTouchTag + 1,
        preTouchTag - 1,
    };
    
    for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
        if (tags[i] == postTag) {
            return true;
        }
    }

    return false;
    
}

void GameScene::swapSprite() {
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
    
    //削除
    m_blockTags[preTouchSprite->getBlockType()].remove(preTouchTag);
    m_blockTags[postTouchSprite->getBlockType()].remove(postTouchTag);
    
    //挿入
    m_blockTags[preTouchSprite->getBlockType()].push_back(postTouchTag);
    m_blockTags[postTouchSprite->getBlockType()].push_back(preTouchTag);
    
}


// タップされたコマのタグを取得
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

// コマ配列にあるか検索
bool GameScene::hasSameColorBlock(list<int> blockTagList, int searchBlockTag)
{
    list<int>::iterator it;
    for (it = blockTagList.begin(); it != blockTagList.end(); ++it)
    {
        if (*it == searchBlockTag)
        {
            return true;
        }
    }
    
    return false;
}

// タップされたコマと同色で且つ接しているコマの配列を返す
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType)
{
    // 同色のコマを格納する配列の初期化
    list<int> sameColorBlockTags;
    sameColorBlockTags.push_back(baseTag);
    
    list<int>::iterator it = sameColorBlockTags.begin();
    while (it != sameColorBlockTags.end())
    {
        int tags[] = {
            *it + 100,
            *it - 100,
            *it + 1,
            *it - 1,
        };
        
        for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        {
            // すでにリストにあるか検索
            if (!hasSameColorBlock(sameColorBlockTags, tags[i]))
            {
                // コマ配列にあるか検索
                if (hasSameColorBlock(m_blockTags[blockType], tags[i]))
                {
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        }
        
        it++;
    }
    
    return sameColorBlockTags;
}

// 配列のコマを削除
void GameScene::removeBlock(list<int> blockTags, kBlock blockType)
{
    bool first = true;
    
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end())
    {
        // 既存配列から該当コマを削除
        m_blockTags[blockType].remove(*it);
        
        // 対象となるコマを取得
        CCNode* block = m_background->getChildByTag(*it);
        if (block)
        {
            // コマが消えるアニメーションを生成
            CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0);
            
            // コマを削除するアクションを生成
            CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));
            
            // アクションをつなげる
            CCFiniteTimeAction* sequence = CCSequence::create(scale, func, NULL);
            
            CCFiniteTimeAction* action;
            if (first)
            {
                // コマが消えるサウンドアクションを生成
                CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);
                
                // アクションをつなげる
                action = CCSpawn::create(sequence, playSe, NULL);
                
                first = false;
            }
            else
            {
                action = sequence;
            }
            
            // アクションをセットする
            block->runAction(action);
        }
        
        
        
        it++;
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}

// コマの削除
void GameScene::removingBlock(CCNode* block)
{
    block->removeFromParentAndCleanup(true);
}

// コマのインデックス取得
GameScene::PositionIndex GameScene::getPositionIndex(int tag)
{
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;
    
    return PositionIndex(pos1_x, pos1_y);
}

// 新しい位置をセット
void GameScene::setNewPosition1(int tag, PositionIndex posIndex)
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

// 消えたコマを埋めるように新しい位置をセット
void GameScene::searchNewPosition1(list<int> blocks)
{
    // 消えるコマ数分のループ
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end())
    {
        PositionIndex posIndex1 = getPositionIndex(*it1);
        
        // コマ種類のループ
        vector<kBlock>::iterator it2 = blockTypes.begin();
        while (it2 != blockTypes.end())
        {
            // 各種類のコマ数分のループ
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            while (it3 != m_blockTags[*it2].end())
            {
                PositionIndex posIndex2 = getPositionIndex(*it3);
                
                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y)
                {
                    // 消えるコマの上に位置するコマに対して、移動先の位置をセットする
                    setNewPosition1(*it3, posIndex2);
                }
                
                it3++;
            }
            
            it2++;
        }
        
        it1++;
    }
}

// コマを移動する
void GameScene::moveBlock()
{
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();
            
            if (nextPosX != -1 || nextPosY != -1)
            {
                // 新しいタグをセットする
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);
                
                // タグ一覧の値も新しいタグに変更する
                *it2 = newTag;
                
                // アニメーションをセットする
                CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                blockSprite->runAction(move);
            }
            
            it2++;
        }
        
        it1++;
    }
}

// コマ削除後のアニメーション
void GameScene::movingBlocksAnimation1(list<int> blocks)
{
    // コマの新しい位置をセットする
    searchNewPosition1(blocks);
    
    // 新しい位置がセットされたコマのアニメーション
    moveBlock();

    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);
}

// コマの移動完了
void GameScene::movedBlocks()
{
    // ラベル再表示
    showLabel();
    
    // アニメーション終了
    m_animating = false;

    // ゲーム終了チェック
    if (!existsSameBlock())
    {
        // ハイスコア記録・表示
        saveHighScore();
        
        CCSize bgSize = m_background->getContentSize();
        
        // ゲーム終了表示
        CCSprite* gameOver = CCSprite::create(PNG_GAMEOVER);
        gameOver->setPosition(ccp(bgSize.width / 2, bgSize.height * 0.8));
        m_background->addChild(gameOver, kZOrderGameOver, kTagGameOver);
        
        setTouchEnabled(false);
    }
}


// 存在する列を取得する
map<int, bool> GameScene::getExistsBlockColumn()
{
    // 検索配列初期化
    map<int, bool> xBlock;
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        xBlock[i] = false;
    }
    
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            // 存在するコマのx 位置を記録
            xBlock[getPositionIndex(*it2).x] = true;
            
            it2++;
        }
        
        it1++;
    }
    
    return xBlock;
}

// ラベル表示
void GameScene::showLabel()
{
    CCSize bgSize = m_background->getContentSize();
    
    // 残数表示
    int tagsForLabel[] = {kTagRedLabel, kTagBlueLabel, kTagYellowLabel, kTagGreenLabel, kTagGrayLabel};
    const char* fontNames[] = {FONT_RED, FONT_BLUE, FONT_YELLOW, FONT_GREEN, FONT_GRAY};
    float heightRate[] = {0.61, 0.51, 0.41, 0.31, 0.21};
    
    // コマ種類のループ
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end())
    {
        // コマ残数表示
        int count = m_blockTags[*it].size();
        const char* countStr = ccsf("%02d", count);
        CCLabelBMFont* label = (CCLabelBMFont*)m_background->getChildByTag(tagsForLabel[*it]);
        if (!label)
        {
            // コマ残数生成
            label = CCLabelBMFont::create(countStr, fontNames[*it]);
            label->setPosition(ccp(bgSize.width * 0.78, bgSize.height * heightRate[*it]));
            m_background->addChild(label, kZOrderLabel, tagsForLabel[*it]);
        }
        else
        {
            label->setString(countStr);
        }
        
        it++;
    }

    // スコア表示
    const char* scoreStr = ccsf("%d", m_score);
    CCLabelBMFont* scoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel)
    {
        // スコア生成
        scoreLabel = CCLabelBMFont::create(scoreStr, FONT_WHITE);
        scoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.75));
        m_background->addChild(scoreLabel, kZOrderLabel, kTagScoreLabel);
    }
    else
    {
        scoreLabel->setString(scoreStr);
    }
}

// 全コマに対して、隣り合うコマが存在するかチェック
bool GameScene::existsSameBlock()
{
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            if (getSameColorBlockTags(*it2, *it1).size() > 1)
            {
                // 隣り合うコマが存在する場合は、trueを返す
                return true;
            }
            
            it2++;
        }
        
        it1++;
    }
    
    // 隣り合うコマが存在しない場合は、falseを返す
    return false;
}

// ハイスコアラベル表示
void GameScene::showHighScoreLabel()
{
    CCSize bgSize = m_background->getContentSize();
    
    // ハイスコア表示
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_HIGHSCORE, 0);
    const char* highScoreStr = ccsf("%d", highScore);
    CCLabelBMFont* highScoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagHighScoreLabel);
    if (!highScoreLabel)
    {
        // ハイスコア生成
        highScoreLabel = CCLabelBMFont::create(highScoreStr, FONT_WHITE);
        highScoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.87));
        m_background->addChild(highScoreLabel, kZOrderLabel, kTagHighScoreLabel);
    }
    else
    {
        highScoreLabel->setString(highScoreStr);
    }
}

// ハイスコア記録・表示
void GameScene::saveHighScore()
{
    CCUserDefault* userDefault = CCUserDefault::sharedUserDefault();
    
    // ハイスコアを取得する
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    if (oldHighScore < m_score)
    {
        // ハイスコアを保持する
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        userDefault->flush();
        
        // ハイスコアを表示する
        showHighScoreLabel();
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
