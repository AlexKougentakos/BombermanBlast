#pragma once
#include "PowerUp.h"

class PowerUp_IncreaseSpeed : public BasePowerUp
{
public:
	PowerUp_IncreaseSpeed(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_IncreaseSpeed() override = default;

	PowerUp_IncreaseSpeed(const PowerUp_IncreaseSpeed& other) = delete;
	PowerUp_IncreaseSpeed(PowerUp_IncreaseSpeed&& other) noexcept = delete;
	PowerUp_IncreaseSpeed& operator=(const PowerUp_IncreaseSpeed& other) = delete;
	PowerUp_IncreaseSpeed& operator=(PowerUp_IncreaseSpeed&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/SpeedUp_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};

class PowerUp_DecreaseSpeed : public BasePowerUp
{
public:
	PowerUp_DecreaseSpeed(GridComponent* pGridComponent, GridCell* pGridCell);
	virtual ~PowerUp_DecreaseSpeed() override = default;

	PowerUp_DecreaseSpeed(const PowerUp_DecreaseSpeed& other) = delete;
	PowerUp_DecreaseSpeed(PowerUp_DecreaseSpeed&& other) noexcept = delete;
	PowerUp_DecreaseSpeed& operator=(const PowerUp_DecreaseSpeed& other) = delete;
	PowerUp_DecreaseSpeed& operator=(PowerUp_DecreaseSpeed&& other) noexcept = delete;


private:
	const std::wstring texturePath{ L"Textures/SpeedDown_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};
