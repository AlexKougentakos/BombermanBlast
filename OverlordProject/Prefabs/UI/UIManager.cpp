#include "stdafx.h"
#include "UIManager.h"
#include "CharacterPointDisplay.h"

UIManager::UIManager(std::vector<BombermanCharacter*> players)
	:m_pPlayers(players)
{

}

void UIManager::Initialize(const SceneContext& sceneContext)
{

	for (size_t i{}; i < m_pPlayers.size(); ++i)
	{
		auto player = m_pPlayers[i];
		AddChild(new CharacterPointDisplay(player, XMFLOAT2{i * (sceneContext.windowWidth / m_pPlayers.size()), sceneContext.windowWidth - 50.f}));

	}
}

void UIManager::Update(const SceneContext&)
{
	
}