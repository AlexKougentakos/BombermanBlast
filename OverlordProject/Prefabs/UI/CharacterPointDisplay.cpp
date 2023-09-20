#include "stdafx.h"
#include "CharacterPointDisplay.h"
#include "Prefabs/BombermanCharacter.h"

CharacterPointDisplay::CharacterPointDisplay(BombermanCharacter* bombermanCharacter, const XMFLOAT2& position):
	m_pBombermanCharacter(bombermanCharacter),
	m_SpritePosition(position)
{
	m_pBombermanCharacter->registerObserver(this);
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
	m_PlayerColour = m_pBombermanCharacter->GetPlayerColour();
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

void CharacterPointDisplay::Update(const SceneContext&)
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(std::to_string(m_ScoreToDisplay)), m_TextPosition);
	m_pFont = ContentManager::Load<SpriteFont>(L"./SpriteFonts/Bomberman.fnt");
}

void CharacterPointDisplay::OnNotify(BombermanCharacter*, const std::string& field)
{
	if (field == "Score Increase")
	{
		++m_ScoreToDisplay;
	}
}
