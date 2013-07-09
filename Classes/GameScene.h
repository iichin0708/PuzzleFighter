#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "Config.h"

#define MAX_BLOCK_X 6
#define MAX_BLOCK_Y 6
#define REMOVING_TIME 0.1f
#define MOVING_TIME 0.2f

#define KEY_HIGHSCORE "HighScore"

#define PNG_BACKGROUND "background.png"
#define PNG_GAMEOVER "gameover.png"
#define PNG_RESET "reset.png"
#define MP3_REMOVE_BLOCK "removeBlock.mp3"


class GameScene : public cocos2d::CCLayer
{
protected:
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
    };
    
    enum kZOrder
    {
        kZOrderBackground,
        kZOrderLabel,
        kZOrderBlock,
        kZOrderGameOver,
    };
    
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
 
    // タッチしたタグ(CCTouchBegan用)
    static int preTouchTag;
    
    // タッチしたタグ(CCTouchMove用)
    static int postTouchTag;
    
    // 消すブロックのリスト
    static std::list<int> removeBlockTagLists;
        
    // アニメーション中のフラグ
    bool m_animating;
    
    // 画像の大きさ
    float m_blockSize;
    
    // スコアを保持
    int m_score;
    
    // 背景画像
    cocos2d::CCSprite* m_background;

    /***** 以下メソッド群 ******/
    
    // 変数を初期化する
    void initForVariables();
    // 背景を表示する
    void showBackground();
    // 初期ブロックを表示する
    void showBlock();
    
    // CCTouchMoveにて取得したタッチポイントが隣接するピースを触ったかどうか
    bool checkCorrectSwap(int preTag, int postTag);
    
    // 2つのスプライトを入れ替える.
    void swapSprite();
    
    // 盤面上で連結のあればパズルを消して、新しいブロックを落とす
    void checkAndRemoveAndDrop();
    
    // 入れ替えアニメーションの終了メソッド
    void exchangeAnimationFinished();

    // 連結していて消滅できるブロックの、タグ配列を取得
    std::list<int> getRemoveChainBlocks();
    
    // 指定したブロックを含む３つ以上のブロック連結があるかどうか
    bool isChainedBlock(int blockTag);
    
    // ブロックを消すアニメーション
    void removeBlocksAniamtion(std::list<int> blockTags, float during);
    
    // 静的変数[removeBlockTagLists]に格納されているタグリストを削除し、新しいブロックを落とす
    void removeAndDrop();
    
    // 与えられたタグの引数リストよりブロックを削除する
    void removeBlock(std::list<int> blockTags);
    
    // 親から実際に参照を削除する
    void removingBlock(cocos2d::CCNode* block);

    // ブロックを下に落とすアニメーション
    void movingBlocksAnimation1(std::list<int> blocks);
    
    // 下にずらすべきブロックを探索する
    void searchNewPosition1(std::list<int> blocks);
    
    // 消されたブロックの上にあるブロックに新しいポジションを設定する(下にずらす)
    void setNewPosition1(int tag, PositionIndex posIndex);
    
    // ブロックを実際に動かす(nextPosが設定してあるブロックのみ)
    void moveBlock();
    
    // 追加したブロックを落とす
    void dropNewBlocks();
    
    // ブロックを下に落とし終わったあとのメソッド
    void movedBlocks();

    // リセットする(初めから始める)
    void menuResetCallback(cocos2d::CCObject* pSender);
    
    // リセットボタンを画面に表示させる
    void showResetButton();
    
    
    /***** 以下ユーティリティ系メソッド *****/
    
    // 与えられたインデックスから画面上のポイントを返す
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);

    // 与えられたインデックスからタグを取得する
    int getTag(int posIndexX, int posIndexY);
    // タッチされたブロックのタグを取得
    void getTouchBlockTag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType);

    // タグからインデックスを取得する
    PositionIndex getPositionIndex(int tag);
    
    // 指定したブロックにある潜在的な連結の数
    int getSwapChainBlockCount(int blockTag);
    
    // 盤面全体の潜在的な連結の数を取得する
    int getSwapChainCount();

    /*********************************/


public:
    
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(GameScene);
    
    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch* pTouch, cocos2d::CCEvent *pEvent);

    virtual void keyBackClicked();
    virtual void keyMenuClicked();
};

#endif // __GAMESCENE_H__#endif // __GAMESCENE_H__