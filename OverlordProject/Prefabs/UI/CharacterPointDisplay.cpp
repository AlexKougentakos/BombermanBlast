#include "stdafx.h"
#include "CharacterPointDisplay.h"
#include "Prefabs/BombermanCharacter.h"

CharacterPointDisplay::CharacterPointDisplay(BombermanCharacter* bombermanCharacter, const XMFLOAT2& position):
	m_pBombermanCharacter(bombermanCharacter),
	m_Position(position)
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

	[[maybe_unused]]std::wstring t{ m_PlayerColourToSpritePathMap[m_PlayerColour] };
	m_pSpriteComponent = AddComponent(new SpriteComponent(m_PlayerColourToSpritePathMap[m_PlayerColour], m_Position));
	
}

void CharacterPointDisplay::Update(const SceneContext&)
{

}

void CharacterPointDisplay::OnNotify(BombermanCharacter*, const std::string& field)
{
	if (field == "Score Increase")
	{
		
	}

	if (field == "Player Death")
	{
		
	}
}
