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
	m_pRoundWinnersSprite->GetTransform()->Translate(centerX - textSize.x / 2,centerY - textSize.y / 2 , 0.9f);
	
	constexpr float margin = 10.f;

	// Create a temporary sprite to get the head size
	GameObject* tempObject = new GameObject();
	const auto tempSprite = tempObject->AddComponent(new SpriteComponent(m_PlayerColourToSpritePathMap[PlayerColour{1}]));
	tempSprite->GetTransform()->Scale(1.33f);
	const XMFLOAT2 headSize = tempSprite->GetDimensions();
	tempObject->RemoveComponent(tempSprite);
	delete tempObject;

	// Calculate total width of all player heads including margins
	const float totalWidth = (headSize.x * m_PlayerIndices.size()) + (margin * (m_PlayerIndices.size() - 1));

	// Calculate starting X position to center all heads
	float startX = centerX - (totalWidth / 2.f);

	for (size_t i = 0; i < m_PlayerIndices.size(); ++i)
	{
		const int playerIndex = m_PlayerIndices[i];
		const PlayerColour playerColour{playerIndex};
    
		GameObject* pPlayerHeadsContainer = new GameObject();
		AddChild(pPlayerHeadsContainer);

		m_pPlayerHead = pPlayerHeadsContainer->AddComponent(new SpriteComponent(m_PlayerColourToSpritePathMap[playerColour]));
		m_pPlayerHead->GetTransform()->Scale(1.33f);

		if (m_PlayerIndices.size() > 1)
		{
			m_pPlayerHead->GetTransform()->Translate(startX, centerY + textSize.y + 5.f, 0.9f);
			startX += headSize.x + margin;
		}
		else 
		{
			m_pPlayerHead->GetTransform()->Translate(centerX - headSize.x / 2, centerY + textSize.y + 5.f, 0.9f);
		}
	}

}

void RoundWinners::Update(const SceneContext&)
{
	
}
