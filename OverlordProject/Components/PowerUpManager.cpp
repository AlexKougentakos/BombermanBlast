#include "stdafx.h"
#include "PowerUpManager.h"

#include "Grid.h"
#include "Prefabs/PowerUps/PowerUp.h"

//Include all the PowerUps here
#include "Prefabs/PowerUps/PowerUp_IncrDecrBlast.h"


PowerUpManager::PowerUpManager(GridComponent* pGridComponent):
m_pGrid(pGridComponent)
{
	
}


void PowerUpManager::Initialize(const SceneContext& /*sceneContext*/)
{

}

void PowerUpManager::TriggerPowerUpSpawn(GridCell& cell)
{
	const bool doesSpawn{ MathHelper::randF(0, 100) <= m_SpawnChance };

	if (!doesSpawn) return;

	const int powerUpType{ rand() % (m_NumOfPowerUps - 1) + 1};

	AddPowerUp(GetPowerUpByType(PowerUpType(powerUpType)), cell);
}

BasePowerUp* PowerUpManager::GetPowerUpByType(PowerUpType type) const
{
	switch (type)
	{
	case PowerUpType::FlameUp:
		return new PowerUp_IncreaseBlast(m_pGrid);
	default:
		return new PowerUp_IncreaseBlast(m_pGrid);
		//assert(true, "wrong power up");
		//return nullptr;
	}
}

void PowerUpManager::AddPowerUp(BasePowerUp* pPowerUp, GridCell& cell)
{
	m_pPowerUps.emplace_back(pPowerUp);
	GetGameObject()->GetComponent<GridComponent>()->PlaceObject(pPowerUp, cell);
}

void PowerUpManager::RemovePowerUp(BasePowerUp* pPowerUp)
{
	m_pPowerUps.erase(std::remove(m_pPowerUps.begin(), m_pPowerUps.end(), pPowerUp), m_pPowerUps.end());

	GetGameObject()->GetComponent<GridComponent>()->DeleteSpecificObject(pPowerUp);
}


void PowerUpManager::UpdatePowerUps()
{
	for (const auto& pPowerUp : m_pPowerUps)
	{
		if (pPowerUp->IsMarkedForRemoval())
		{
			RemovePowerUp(pPowerUp);
			break;
		}
	}
}
