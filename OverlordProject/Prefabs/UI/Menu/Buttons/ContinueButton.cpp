#include "stdafx.h"
#include "ContinueButton.h"

ContinueButton::ContinueButton(const XMFLOAT2& pos, const std::wstring& coverImagePath) :
	Button(pos, coverImagePath)
{
	m_NormalColour = m_pSpriteComponent->GetColor();
	m_HoverColour = XMFLOAT4{ m_NormalColour.x - 0.2f, m_NormalColour.y - 0.2f, m_NormalColour.z - 0.2f, 1.f };
}

void ContinueButton::OnClick() const
{
	
}

void ContinueButton::OnHover() const
{
	m_pSpriteComponent->SetColor(m_HoverColour);
}

void ContinueButton::OnHoverExit() const
{
	m_pSpriteComponent->SetColor(m_NormalColour);
}

