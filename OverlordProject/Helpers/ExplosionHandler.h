#pragma once
#include "Visitor.h"

// Here we define the fixed arguments as, for example, int and float. Change them as you see fit.
class ExplosionHandler final : public Visitor<GridCell*>
{
public:
    void Visit(BombermanCharacter* bomberman, GridCell* gridCell) override;
    void Visit(BasePowerUp* powerUp, GridCell* gridCell) override;
    void Visit(RockPrefab* rock, GridCell* gridCell) override;
};
