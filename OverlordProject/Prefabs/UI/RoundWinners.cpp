#include "stdafx.h"
#include "RoundWinners.h"

void RoundWinners::Initialize(const SceneContext&)
{
	m_pRoundOverSprite = AddComponent(new SpriteComponent(L"Textures/UI/RoundOver.png"));
}

void RoundWinners::Update(const SceneContext&)
{
	
}
