#pragma once
#include "PowerUp.h"

class PowerUp_IncreaseBlast : public BasePowerUp
{
public:
	PowerUp_IncreaseBlast(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_IncreaseBlast() override = default;

	PowerUp_IncreaseBlast(const PowerUp_IncreaseBlast& other) = delete;
	PowerUp_IncreaseBlast(PowerUp_IncreaseBlast&& other) noexcept = delete;
	PowerUp_IncreaseBlast& operator=(const PowerUp_IncreaseBlast& other) = delete;
	PowerUp_IncreaseBlast& operator=(PowerUp_IncreaseBlast&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/FireUp_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};

class PowerUp_DecreaseBlast : public BasePowerUp
{
public:
	PowerUp_DecreaseBlast(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_DecreaseBlast() override = default;

	PowerUp_DecreaseBlast(const PowerUp_DecreaseBlast& other) = delete;
	PowerUp_DecreaseBlast(PowerUp_DecreaseBlast&& other) noexcept = delete;
	PowerUp_DecreaseBlast& operator=(const PowerUp_DecreaseBlast& other) = delete;
	PowerUp_DecreaseBlast& operator=(PowerUp_DecreaseBlast&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/FireDown_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};
