#include "cocos2d.h"
#include "Player.h"

Player::Player(int maxHitPoint, int maxMagicPoint, int offencePower, int healingPower, int magicChargePower)
{
    // 体力
    hpGauge = Gauge::create(maxHitPoint);
    
    // スキルポイント
    magicGauge = Gauge::create(maxMagicPoint);
    magicGauge->decrease(maxMagicPoint);
    
    // 攻撃力
    this->offencePower = offencePower;
    
    // 回復力
    this->healingPower = healingPower;
    
    // スキルポイントチャージ力
    this->magicChargePower = magicChargePower;
}

// 攻撃(現時点では与えるダメージ数をログで出力)
void Player::attack(int offenceEnergy)
{
    CCLOG("Attack! %d point", offencePower * offenceEnergy);
}

// 被ダメージ
void Player::damage(int damagePoint)
{
    hpGauge->decrease(damagePoint);
}

// 回復
void Player::heal(int healEnergy)
{
    hpGauge->increase(healingPower * healEnergy);
}

// スキルポイントチャージ
void Player::chargeMagicPoint(int magicEnergy)
{
    magicGauge->increase(magicChargePower * magicEnergy);
}