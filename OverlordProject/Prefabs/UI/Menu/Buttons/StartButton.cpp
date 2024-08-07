#include "stdafx.h"
#include "StartButton.h"

StartButton::StartButton(const XMFLOAT2& pos, const std::wstring& coverImagePath):
Button(pos, coverImagePath)
{
	m_NormalColour = m_pSpriteComponent->GetColor();
	m_HoverColour = XMFLOAT4{ m_NormalColour.x - 0.2f, m_NormalColour.y - 0.2f, m_NormalColour.z - 0.2f, 1.f };
}

void StartButton::OnClick() const
{
	SceneManager::Get()->SetActiveGameScene(L"BombermanBlastScene");
}

void StartButton::OnHover() const
{
	m_pSpriteComponent->SetColor( m_HoverColour);
}

void StartButton::OnHoverExit() const
{
	m_pSpriteComponent->SetColor(m_NormalColour);
}