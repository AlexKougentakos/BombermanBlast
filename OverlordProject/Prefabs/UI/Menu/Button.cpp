#include "stdafx.h"
#include "Button.h"

Button::Button(const XMFLOAT2& pos, const std::wstring& coverImagePath):
	m_Position(pos)
{
	m_pSpriteComponent = AddComponent(new SpriteComponent(coverImagePath));
}

void Button::Initialize(const SceneContext&)
{

}

void Button::PostInitialize(const SceneContext&)
{
	const float halfWidth = m_pSpriteComponent->GetDimensions().x / 2.f;
	const float halfHeight = m_pSpriteComponent->GetDimensions().y / 2.f;

	m_pSpriteComponent->GetTransform()->Translate(XMFLOAT3{ m_Position.x - halfWidth, m_Position.y - halfHeight, 0.3f });
}
