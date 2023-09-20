#include "stdafx.h"
#include "PowerUp_IncrDecrSpeed.h"

#include "Prefabs/BombermanCharacter.h"

PowerUp_IncreaseSpeed::PowerUp_IncreaseSpeed(GridComponent* pGridComponent)
	:BasePowerUp(pGridComponent)
{
	SetTexture(texturePath);

}

void PowerUp_IncreaseSpeed::OnCollision(GameObject*, GameObject* other, PxTriggerAction action)
{
	if (action == PxTriggerAction::LEAVE) return;

	const auto player = dynamic_cast<BombermanCharacter*>(other);

	if (!player) return;

	PlayerStatIncrease increase{};
	increase.speed = 1;

	player->ApplyPowerup(increase);

	MarkForRemoval();
}


PowerUp_DecreaseSpeed::PowerUp_DecreaseSpeed(GridComponent* pGridComponent)
	:BasePowerUp(pGridComponent)
{
	SetTexture(texturePath);

}

void PowerUp_DecreaseSpeed::OnCollision(GameObject*, GameObject* other, PxTriggerAction action)
{
	if (action == PxTriggerAction::LEAVE) return;

	const auto player = dynamic_cast<BombermanCharacter*>(other);

	if (!player) return;

	PlayerStatIncrease increase{};
	increase.speed = -1;

	player->ApplyPowerup(increase);

	MarkForRemoval();
}
