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
		const auto player = m_pPlayers[i];
		constexpr int maxPlayers{4};
		const float increment{ sceneContext.windowWidth / maxPlayers + 1 };

		AddChild(new CharacterPointDisplay(player, XMFLOAT2{i * increment, 0.f}));
	}
}

void UIManager::Update(const SceneContext&)
{
	
}