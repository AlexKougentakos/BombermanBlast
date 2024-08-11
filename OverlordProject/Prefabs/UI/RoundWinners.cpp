#include "stdafx.h"
#include "RoundWinners.h"

#include "Prefabs/BombermanCharacter.h"

RoundWinners::RoundWinners(const std::vector<int>& playerIndices)
{
	m_PlayerIndices = playerIndices;
}

void RoundWinners::Initialize(const SceneContext& sceneContext)
{

	m_PlayerColourToSpritePathMap =
{
		{PlayerColour::BLUE, L"Textures/UI/BlueBomberMan_HeadSprite.png"},
		{PlayerColour::RED, L"Textures/UI/RedBomberMan_HeadSprite.png"},
		{PlayerColour::WHITE, L"Textures/UI/WhiteBomberMan_HeadSprite.png"},
		{PlayerColour::YELLOW, L"Textures/UI/YellowBomberMan_HeadSprite.png"}
};

	const float centerX = sceneContext.windowWidth / 2.f;
	const float centerY = sceneContext.windowHeight / 2.f;

	GameObject* pRoundWinnerContainer = new GameObject();
	AddChild(pRoundWinnerContainer);
	m_pRoundWinnersSprite = pRoundWinnerContainer->AddComponent(new SpriteComponent(L"Textures/UI/RoundWinners.png"));
	const XMFLOAT2 textSize = m_pRoundWinnersSprite->GetDimensions();
	m_pRoundWinnersSprite->GetTransform()->Translate(centerX - textSize.x/2,centerY - textSize.y/2 , 0.9f);

	GameObject* pPlayerHeadsContainer = new GameObject();
	AddChild(pPlayerHeadsContainer);
	for (const int playerIndex : m_PlayerIndices)
	{
		const PlayerColour playerColour{playerIndex};
		m_pPlayerHead = pPlayerHeadsContainer->AddComponent(new SpriteComponent(m_PlayerColourToSpritePathMap[playerColour]));
		const XMFLOAT2 headSize = m_pPlayerHead->GetDimensions();
		m_pPlayerHead->GetTransform()->Translate(centerX - (textSize.x - headSize.x) / 2 + (headSize.x + 5.f) * (playerIndex - 1), centerY + textSize.y + 5.f , 0.9f);
	}

}

void RoundWinners::Update(const SceneContext&)
{
	
}
