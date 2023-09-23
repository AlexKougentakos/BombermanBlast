#pragma once
#include "Visitor.h"

struct GridCell;

class RockPrefab;
class BasePowerUp;


//Simple Helper Class to assign cells to placed object using the Visitor Pattern
class CellAssigner final : public Visitor<GridCell*>
{
public:
	void Visit(BasePowerUp* powerUp, GridCell* gridCellToAssign) override;
	void Visit(RockPrefab* rock, GridCell* gridCellToAssign) override;
};

