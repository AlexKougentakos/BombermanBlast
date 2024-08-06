#include "stdafx.h"
#include "PowerUpManager.h"

#include "Grid.h"
#include "Prefabs/PowerUps/PowerUp.h"

//Include all the PowerUps here
#include "Prefabs/PowerUps/PowerUp_IncrDecrBlast.h"
#include "Prefabs/PowerUps/PowerUp_IncrDecrBombs.h"
#include "Prefabs/PowerUps/PowerUp_IncrDecrSpeed.h"
#include "GameObjectManager.h"


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

	AddPowerUp(GetPowerUpByType(PowerUpType(powerUpType), &cell), cell);
}

BasePowerUp* PowerUpManager::GetPowerUpByType(PowerUpType type, GridCell* pGridCell) const
{
	switch (type)
	{
	case PowerUpType::FlameUp:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_IncreaseBlast>(m_pGrid, pGridCell);
	case PowerUpType::FlameDown:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_DecreaseBlast>(m_pGrid, pGridCell);
	case PowerUpType::BombUp:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_IncreaseBomb>(m_pGrid, pGridCell);
	case PowerUpType::BombDown:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_DecreaseBomb>(m_pGrid, pGridCell);
	case PowerUpType::SpeedUp:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_IncreaseSpeed>(m_pGrid, pGridCell);
	case PowerUpType::SpeedDown:
		return m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<PowerUp_DecreaseSpeed>(m_pGrid, pGridCell);
	default:
		assert(true && "wrong power up");
		return nullptr;
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

	GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(pPowerUp);
	
}

void PowerUpManager::RemovePowerUps()
{
	for (const auto& pPowerUp : m_pPowerUps)
	{
		RemovePowerUp(pPowerUp);
	}
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
