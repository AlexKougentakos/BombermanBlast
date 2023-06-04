#include "stdafx.h"
#include "ControllsButton.h"

ControllButton::ControllButton(const XMFLOAT2& pos, const std::wstring& coverImagePath) :
	Button(pos, coverImagePath)
{
	m_NormalColour = m_pSpriteComponent->GetColor();
	m_HoverColour = XMFLOAT4{ m_NormalColour.x - 0.2f, m_NormalColour.y - 0.2f, m_NormalColour.z - 0.2f, 1.f };
}

void ControllButton::OnClick() const
{
	SceneManager::Get()->SetActiveGameScene(L"ControlsScene");
}

void ControllButton::OnHover() const
{
	m_pSpriteComponent->SetColor(m_HoverColour);

}

void ControllButton::OnHoverExit() const
{
	m_pSpriteComponent->SetColor(m_NormalColour);
}

