#include "stdafx.h"
#include "ButtonManager.h"

#include <thread>

#include "Button.h"

ButtonManager::ButtonManager(const XMFLOAT2* pCustomCursor)
{
	if (!pCustomCursor) return;

	m_pCustomCursorPos = pCustomCursor;
}

void ButtonManager::Initialize(const SceneContext& sceneContext)
{
    m_SceneContext = sceneContext;

	m_ControllerClickId = sceneContext.pInput->NumberOfActions() + 1;
 	sceneContext.pInput->AddInputAction(InputAction(sceneContext.pInput->NumberOfActions() + 1, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A));
}

void ButtonManager::Update(const SceneContext&)
{
	UpdateButtons(XMFLOAT2{ static_cast<float>(InputManager::GetMousePosition().x),
	static_cast<float>(InputManager::GetMousePosition().y) }, XMFLOAT2{*m_pCustomCursorPos});
}


void ButtonManager::AddButton(Button* pButton)
{
	AddChild(pButton);

	m_pButtons.emplace_back(pButton);
}

void ButtonManager::UpdateButtons(const XMFLOAT2&, const XMFLOAT2& customCursor)
{
    for (const auto button : m_pButtons)
    {
        const bool isHoveringCustomCursor = button->GetTransform()->GetPosition().x < customCursor.x
            && button->GetTransform()->GetPosition().x + button->GetWidth() > customCursor.x
            && button->GetTransform()->GetPosition().y < customCursor.y
            && button->GetTransform()->GetPosition().y + button->GetHeight() > customCursor.y;

        if (isHoveringCustomCursor)
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
                !InputManager::IsMouseButton(InputState::pressed, 1) &&
                !m_SceneContext.pInput->IsActionTriggered(m_ControllerClickId)) return;

            

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