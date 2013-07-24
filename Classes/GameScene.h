#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "Config.h"
#include "BlockSprite.h"
#include "Player.h"

#define MAX_BLOCK_X 7
#define MAX_BLOCK_Y 6
#define SWAPPING_TIME 0.2f
#define REMOVING_TIME 0.1f
#define MOVING_TIME 0.2f
#define HINT_TIME 4.0f
#define COMBO_TIME 1.5f
#define FEVER_COUNT 10
#define DEFAULT_PLAY_TIME 3600

#define KEY_HIGHSCORE "HighScore"

#define LABEL_READY_IN_TIME 1.0f
#define LABEL_READY_OUT_TIME 0.5f

#define LABEL_GO_IN_TIME 0.2f
#define LABEL_GO_OUT_TIME 0.3f

#define LABEL_TIMEUP_IN_TIME 2.0f
#define LABEL_TIMEUP_OUT_TIME 0.5f

#define LABEL_FEVER_IN_TIME 0.5f
#define LABEL_FEVER_OUT_TIME 0.7f

#define DELETE_LINE_TIME 0.5f

//#define PNG_BACKGROUND "back_ground.png"
#define PNG_BACKGROUND "back.png"
#define PNG_GAMEOVER "gameover.png"
//#define PNG_RESET "reset.png"
//#define PNG_RESET "pause_button.png"
#define PNG_RESET "pause_button_reverse.png"
#define MP3_REMOVE_BLOCK "removeBlock.mp3"
#include "Timer.h"
#include "PowerUpSprite.h"


class GameScene : public cocos2d::CCLayer
{
protected:
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
    
    // アラートフラグ
    bool isShowedAlert;
    
    // フィーバーフラグ
    bool isStartFever;
    
    // 画像の大きさ
    float m_blockSize;
    
    // タイマー
    Timer *timer;
    
    // プレイヤーのパラメータを扱うオブジェクト
    Player *player;
    
    // 獲得コイン数
    int coin;
    
    // 前回のコンボ数
    int preCombo;
    
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
    
    void setPreDrop();
    
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
    
    // アニメーションのキャッシュの追加
    void addAnimationCache(const char *fileName, const char *cacheName, int startNum, int endNum , bool isReverse, float duration);

    // アニメーションの登録
    void signUpAnimation();

    void showGameIntroReady();
    
    void showGameIntroGo();
    
    void showGameIntroFinished();
    
    void setGameOver();
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
        kTagTimer,
        kTagTimerGauge,
        kTagTimerNumber,
        kTagGameOver,
        kTagBaseBlock = 10000,
        kTagHintCircle = 15000,
        kTagComboNumber = 20000,
        kTagComboText,
        kTagComboGauge,
        kTagComboFrame,
        kTagScoreNumber = 25000,
        kTagAlert,
        kTagFever,
    };

    enum kZOrder
    {
        kZOrderBackground,
        kZOrderLabel,
        kZOrderBlock,
        kZOrderCombo,
        kZOrderScore,
        kZOrderTimer,
        kZOrderTimerLabel,
        kZOrderAlert,
        kZOrderFever,
        kZOrderGameSectionLabel,
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
    
    // アニメーションの取得
    CCSprite* getAnimation(char* animName);

    int getRemoveColors(std::list<int> removeBlockTags);
        
    bool allMoved;
    
    // コンボ数
    int m_combo;
    
    std::list<PowerUpSprite*> managePowerList;
    
    // コンボ数のリセット
    void resetCombo();
    
    void setDeleteType(std::list<int> removeBlockColorTags);
    
    void searchAndSetDeleteType(std::list<int> removeBlockTags);

    // コンボ数の演出
    void showCombo();
    
    // スコアを保持
    int m_score;
    
    // スコアを更新する.
    void updateScore();

    // ゲーム終了
    void timeUp();
    
    // 残り時間が5秒以内を警告する
    void showAlert();
    
    // フィーバーをスタートする
    void startFever();
    
    // 引数の場所を中心に行列を消すアニメーション
    void lineDeleteAnimation(int indexX, int indexY);
    
    std::list<int> seekChainRecursive(std::list<int> &tes, int baseTag, int up);
    
#pragma mark アニメーションと同時に消すほうがいいかもね
    // 行列を削除する.
    void linearDelete(int indexX, int indexY);
    
    // 周囲を削除する.
    void aroundDelete(int indexX, int indexY);
    
};

#endif // __GAMESCENE_H__#endif // __GAMESCENE_H__