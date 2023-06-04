#include "stdafx.h"
#include "ButtonManager.h"
#include "Button.h"


ButtonManager::ButtonManager()
{
	
}

void ButtonManager::Initialize(const SceneContext&)
{

}

void ButtonManager::Update(const SceneContext&)
{
		UpdateButtons(XMFLOAT2{static_cast<float>(InputManager::GetMousePosition().x),
			static_cast<float>(InputManager::GetMousePosition().y)});
}


void ButtonManager::AddButton(Button* pButton)
{
	m_pButtons.emplace_back(pButton);
}

void ButtonManager::UpdateButtons(const XMFLOAT2& mousePos)
{
	for (const auto button : m_pButtons)
	{
		if (button->GetTransform()->GetPosition().x < mousePos.x && button->GetTransform()->GetPosition().x + button->GetWidth() > mousePos.x 
			&& button->GetTransform()->GetPosition().y < mousePos.y && button->GetTransform()->GetPosition().y + button->GetHeight() > mousePos.y)
		{
			if (!m_ExecutedOnHoverEnter)
			{
				button->OnHover();
				button->SetIsHovering(true);
				SoundManager::Get()->Play(L"Menu_Hover.mp3");

				m_ExecutedOnHoverEnter = true;
				m_ExecutedOnHoverExit = false;
			}

			if (!InputManager::IsMouseButton(InputState::pressed, 2) &&
				!InputManager::IsMouseButton(InputState::pressed, 1)) return;

			//Todo: test controller input with InputManager::IsGamepadButton()

			button->OnClick();
			SoundManager::Get()->Play(L"Menu_Click.mp3");
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //Give the sound some time to play
		}

		else if (button->IsHovering())
		{
			button->SetIsHovering(false);

			m_ExecutedOnHoverEnter = false;
			m_ExecutedOnHoverExit = true;

			button->OnHoverExit();
		}
	}
}