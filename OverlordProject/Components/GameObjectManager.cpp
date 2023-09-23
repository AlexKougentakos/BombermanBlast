#include "stdafx.h"
#include "GameObjectManager.h"
#include "../Components/Grid.h"

void GameObjectManager::RemoveGameObject(GameObject* pGameObject)
{
	m_pGameObjects.erase(std::remove(m_pGameObjects.begin(), m_pGameObjects.end(), pGameObject), m_pGameObjects.end());

	//Here would go the step of the memory pool, could return a bool and pass it in the remove child to delete it as well
	GetScene()->RemoveChild(pGameObject, true);
}