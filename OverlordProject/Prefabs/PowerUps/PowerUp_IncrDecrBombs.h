#pragma once
#include "PowerUp.h"

class PowerUp_IncreaseBomb : public BasePowerUp
{
public:
	PowerUp_IncreaseBomb(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_IncreaseBomb() override = default;

	PowerUp_IncreaseBomb(const PowerUp_IncreaseBomb& other) = delete;
	PowerUp_IncreaseBomb(PowerUp_IncreaseBomb&& other) noexcept = delete;
	PowerUp_IncreaseBomb& operator=(const PowerUp_IncreaseBomb& other) = delete;
	PowerUp_IncreaseBomb& operator=(PowerUp_IncreaseBomb&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/BombUp_Texture.png" };


	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};

class PowerUp_DecreaseBomb : public BasePowerUp
{
public:
	PowerUp_DecreaseBomb(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_DecreaseBomb() override = default;

	PowerUp_DecreaseBomb(const PowerUp_DecreaseBomb& other) = delete;
	PowerUp_DecreaseBomb(PowerUp_DecreaseBomb&& other) noexcept = delete;
	PowerUp_DecreaseBomb& operator=(const PowerUp_DecreaseBomb& other) = delete;
	PowerUp_DecreaseBomb& operator=(PowerUp_DecreaseBomb&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/BombDown_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};
