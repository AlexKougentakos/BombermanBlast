#include "stdafx.h"
#include "PowerUp_IncrDecrBombs.h"

#include "Prefabs/BombermanCharacter.h"

PowerUp_IncreaseBomb::PowerUp_IncreaseBomb(GridComponent* pGridComponent, GridCell* pGridCell)
	:BasePowerUp(pGridComponent, pGridCell)
{
	SetTexture(texturePath);

}

void PowerUp_IncreaseBomb::OnCollision(GameObject*, GameObject* other, PxTriggerAction action)
{
	if (action == PxTriggerAction::LEAVE) return;

	const auto player = dynamic_cast<BombermanCharacter*>(other);

	if (!player) return;

	PlayerStatIncrease increase{};
	increase.bombs = 1;

	player->ApplyPowerup(increase);

	MarkForRemoval();
}


PowerUp_DecreaseBomb::PowerUp_DecreaseBomb(GridComponent* pGridComponent, GridCell* pGridCell)
	:BasePowerUp(pGridComponent, pGridCell)
{
	SetTexture(texturePath);

}

void PowerUp_DecreaseBomb::OnCollision(GameObject*, GameObject* other, PxTriggerAction action)
{
	if (action == PxTriggerAction::LEAVE) return;

	const auto player = dynamic_cast<BombermanCharacter*>(other);

	if (!player) return;

	PlayerStatIncrease increase{};
	increase.bombs = -1;

	player->ApplyPowerup(increase);

	MarkForRemoval();
}
