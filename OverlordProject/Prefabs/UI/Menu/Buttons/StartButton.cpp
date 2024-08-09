#include "stdafx.h"
#include "StartButton.h"

#include "Scenes/GameScene/BombermanBlastScene.h"

StartButton::StartButton(const XMFLOAT2& pos, const std::wstring& coverImagePath):
Button(pos, coverImagePath)
{
	m_NormalColour = m_pSpriteComponent->GetColor();
	m_HoverColour = XMFLOAT4{ m_NormalColour.x - 0.2f, m_NormalColour.y - 0.2f, m_NormalColour.z - 0.2f, 1.f };
}

void StartButton::OnClick() const
{
	//Remove the scene if it already exists
	if (GameScene* scene = SceneManager::Get()->GetGameScene(L"BombermanBlastScene"))
	{
		SceneManager::Get()->RemoveGameScene(scene, true);
	}

	//Create a new one
	SceneManager::Get()->AddGameScene(new BombermanBlastScene());
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