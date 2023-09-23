#include "stdafx.h"
#include "CellAssigner.h"

#include "Prefabs/PowerUps/PowerUp.h"
#include "Prefabs/RockPrefab.h"

void CellAssigner::Visit(BasePowerUp* /*powerUp*/, GridCell* /*gridCellToAssign*/)
{
	
}

void CellAssigner::Visit(RockPrefab* rock, GridCell* gridCellToAssign)
{
	rock->AssignCell(gridCellToAssign);
}