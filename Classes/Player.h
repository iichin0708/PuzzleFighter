#ifndef __PuzzleFighter__Player__
#define __PuzzleFighter__Player__

#include "Config.h"
#include "Gauge.h"

class Player
{
protected:
    // 攻撃力
    int offencePower;
    
    // 回復力
    int healingPower;
    
    // スキルポイントチャージ力
    int magicChargePower;
    
public:
    Player(int maxHitPoint, int maxMagicPoint, int offencePower, int healingPower, int magicChargePower);
    
    // 体力
    Gauge *hpGauge;
    
    // スキルポイント
    Gauge *magicGauge;
    
    // 攻撃
    void attack(int offenceEnergy);
    
    // 被ダメージ
    void damage(int damagePoint);
    
    // 回復
    void heal(int healEnergy);
    
    // スキルポイントチャージ
    void chargeMagicPoint(int magicEnergy);
};


#endif /* defined(__PuzzleFighter__Player__) */
