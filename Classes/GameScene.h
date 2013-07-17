#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "Config.h"
#include "BlockSprite.h"
#include "Player.h"

#define MAX_BLOCK_X 7
#define MAX_BLOCK_Y 6
#define SWAPPING_TIME 0.1f
#define REMOVING_TIME 0.3f
#define MOVING_TIME 0.2f
#define HINT_TIME 4.0f
#define COMBO_TIME 1.0f

#define KEY_HIGHSCORE "HighScore"

//#define PNG_BACKGROUND "back_ground.png"
#define PNG_BACKGROUND "back.png"
#define PNG_GAMEOVER "gameover.png"
//#define PNG_RESET "reset.png"
//#define PNG_RESET "pause_button.png"
#define PNG_RESET "pause_button_reverse.png"
#define MP3_REMOVE_BLOCK "removeBlock.mp3"



class GameScene : public cocos2d::CCLayer
{
protected:
    /*
    struct PositionIndex
    {
        PositionIndex(int x1, int y1)
        {
            x = x1;
            y = y1;
        }
        
        int x;
        int y;
    };
     */
 
    // ブロックタグを2つセットで扱う
    struct BlockTagPair
    {
        int tag1, tag2;
        BlockTagPair(int tag1, int tag2)
        {
            this->tag1 = tag1;
            this->tag2 = tag2;
        }
    };
    
    // タッチしたタグ(CCTouchBegan用)
    static int preTouchTag;
    
    // タッチしたタグ(CCTouchMove用)
    static int postTouchTag;
    
    // ブロックをスワップしたかどうか
    bool m_isSwappedBlocks;
    
    // 画像の大きさ
    float m_blockSize;
    
    // スコアを保持
    int m_score;
    
    // プレイヤーのパラメータを扱うオブジェクト
    Player *player;
    
    // 獲得コイン数
    int coin;
    
    // コイン数表示ラベル
    CCLabelTTF *coinCount;

    /***** 以下メソッド群 ******/
        
    // 変数を初期化する
    void initForVariables();
    // 背景を表示する
    void showBackground();
    // 初期ブロックを表示する
    void showBlock();
    
    // CCTouchMoveにて取得したタッチポイントが隣接するピースを触ったかどうか
    bool checkCorrectSwap(int preTag, int postTag);
    
    // 盤面上で連結のあればパズルを消して、新しいブロックを落とす
    void checkAndRemoveAndDrop();
    
    // リセットする(初めから始める)
    void menuResetCallback(cocos2d::CCObject* pSender);
    
    // リセットボタンを画面に表示させる
    void showResetButton();
    
    void setNewPosition();
    /***** 以下ユーティリティ系メソッド *****/
    // タッチされたブロックのタグを取得
    void getTouchBlockTag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType);

    // タグからインデックスを取得する
    BlockSprite::PositionIndex getPositionIndex(int tag);
    
    // 指定したブロックにある潜在的な連結の数
    int getSwapChainBlockCount(int blockTag);
    
    // 盤面全体の潜在的な連結の数を取得する
    int getSwapChainCount();
        
    // ヒント（入れ替えで連結）の場所リストを取得
    std::list<BlockTagPair> getSwapChainPositions();
    
    // コンボ数の演出
    void showCombo();


    /*********************************/


    



public:
    enum kTag
    {
        kTagBackground = 1,
        kTagRedLabel,
        kTagBlueLabel,
        kTagYellowLabel,
        kTagGreenLabel,
        kTagGrayLabel,
        kTagScoreLabel,
        kTagGameOver,
        kTagBaseBlock = 10000,
        kTagHintCircle = 20000
    };

    enum kZOrder
    {
        kZOrderBackground,
        kZOrderLabel,
        kZOrderBlock,
        kZOrderGameOver,
    };
    
    virtual bool init();
    static cocos2d::CCScene* scene();
    static bool addFlag;
    
    CREATE_FUNC(GameScene);
    
    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);

    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    // 与えられたインデックスからタグを取得する
    int getTag(int posIndexX, int posIndexY);
    
    // 連鎖があるブロックを消去する.
    void removeChainBlocks();
    
    // 指定したブロックより上にあるブロックの新しいポジションを設定
    void setDropNewPosition(BlockSprite *bSprite);
    
    // ヒントをランダムに1つ表示
    void showSwapChainPosition();
    
    // 背景画像
    cocos2d::CCSprite* m_background;
    
    // 与えられたインデックスから画面上のポイントを返す
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);
    
    std::list<int> checkChain(BlockSprite *bSprite);
    
    void removeBlocks(std::list<int> removeBlockTags);
    
    void addBlocks();

    void recursiveCheck();
    
    void setDeletingFlags(std::list<int> removeBlockTags);
    
    int getRemoveColors(std::list<int> removeBlockTags);
        
    bool allMoved;
    
    // コンボ数
    int m_combo;

    // コンボ数のリセット
    void resetCombo();
};

#endif // __GAMESCENE_H__#endif // __GAMESCENE_H__