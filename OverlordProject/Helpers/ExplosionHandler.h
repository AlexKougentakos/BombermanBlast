#pragma once
#include "Visitor.h"

class ExplosionHandler final : public Visitor<GridCell*>
{
public:
    void Visit(BombermanCharacter* bomberman, GridCell* gridCell) override;
    void Visit(BasePowerUp* powerUp, GridCell* gridCell) override;
    void Visit(RockPrefab* rock, GridCell* gridCell) override;
};
