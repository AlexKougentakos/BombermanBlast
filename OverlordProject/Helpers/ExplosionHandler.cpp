#include "stdafx.h"
#include "ExplosionHandler.h"

#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/RockPrefab.h"
#include "Prefabs/PowerUps/PowerUp.h"

void ExplosionHandler::Visit(BombermanCharacter* character)
{
	character->Kill();
}

void ExplosionHandler::Visit(BasePowerUp* /*powerUp*/)
{
	//todo: add the option to destroy the powerup later
	std::cout << "PowerUp destroyed" << std::endl;
}

void ExplosionHandler::Visit(RockPrefab* rock)
{
	rock->Explode();
}