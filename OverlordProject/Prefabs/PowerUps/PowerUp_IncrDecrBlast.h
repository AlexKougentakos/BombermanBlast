#pragma once
#include "PowerUp.h"

class PowerUp_IncreaseBlast : public BasePowerUp
{
public:
	PowerUp_IncreaseBlast(GridComponent* pGridComponent);
	virtual ~PowerUp_IncreaseBlast() override = default;

	PowerUp_IncreaseBlast(const PowerUp_IncreaseBlast& other) = delete;
	PowerUp_IncreaseBlast(PowerUp_IncreaseBlast&& other) noexcept = delete;
	PowerUp_IncreaseBlast& operator=(const PowerUp_IncreaseBlast& other) = delete;
	PowerUp_IncreaseBlast& operator=(PowerUp_IncreaseBlast&& other) noexcept = delete;

	void Update(const SceneContext&) override;

private:
	const std::wstring texturePath{ L"Textures/FireUp_Texture.png" };

	void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) override;
};
