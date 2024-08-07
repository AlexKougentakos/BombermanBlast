#include "stdafx.h"
#include "CharacterPointDisplay.h"
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/GameLoopManager.h"

CharacterPointDisplay::CharacterPointDisplay(const int characterIndex, GameLoopManager* pGameLoopManager, const XMFLOAT2& position):
	m_CharacterIndex(characterIndex),
	m_pGameLoopManager(pGameLoopManager),
	m_SpritePosition(position)
{
	pGameLoopManager->registerObserver(this);
}

void CharacterPointDisplay::Initialize(const SceneContext&)
{
	m_PlayerColourToSpritePathMap =
	{
		{PlayerColour::BLUE, L"Textures/UI/BlueBomberMan_HeadSprite.png"},
		{PlayerColour::RED, L"Textures/UI/RedBomberMan_HeadSprite.png"},
		{PlayerColour::WHITE, L"Textures/UI/WhiteBomberMan_HeadSprite.png"},
		{PlayerColour::YELLOW, L"Textures/UI/YellowBomberMan_HeadSprite.png"}
	};

	//Player Head
	m_PlayerColour = PlayerColour(m_CharacterIndex);
	m_pSpriteComponent = AddComponent(new SpriteComponent(m_PlayerColourToSpritePathMap[m_PlayerColour]));
	m_pSpriteComponent->GetTransform()->Translate(m_SpritePosition.x, m_SpritePosition.y , 0.9f);

	//Score
	m_pFont = ContentManager::Load<SpriteFont>(L"./SpriteFonts/Bomberman.fnt");

	constexpr float offset{ 10.f };
	m_TextPosition = XMFLOAT2
	{
		m_SpritePosition.x + m_pSpriteComponent->GetDimensions().x + offset,
		m_SpritePosition.y +	5.f
	};
}

void CharacterPointDisplay::Draw(const SceneContext&)
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(std::to_string(m_ScoreToDisplay)), m_TextPosition);
	
}


void CharacterPointDisplay::OnNotify(GameLoopManager*, const std::string& field)
{
	if (field == "Player Score Increase")
	{
		m_ScoreToDisplay = m_pGameLoopManager->GetPlayerScores()[m_CharacterIndex];
	}
}
