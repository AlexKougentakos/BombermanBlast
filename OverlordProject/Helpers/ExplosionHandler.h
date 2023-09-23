#pragma once
#include "Visitor.h"

// Here we define the fixed arguments as, for example, int and float. Change them as you see fit.
class ExplosionHandler final : public Visitor<>
{
public:
    void Visit(BombermanCharacter* bomberman) override;
    void Visit(BasePowerUp* powerUp) override;
    void Visit(RockPrefab* rock) override;
};
