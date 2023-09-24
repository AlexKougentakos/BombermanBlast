#include "stdafx.h"
#include "UIManager.h"
#include "CharacterPointDisplay.h"
#include "Timer.h"

UIManager::UIManager(std::vector<BombermanCharacter*> players)
	:m_pPlayers(players)
{

}

void UIManager::Initialize(const SceneContext& sceneContext)
{
    for (size_t i{}; i < m_pPlayers.size(); ++i)
    {
        const auto player = m_pPlayers[i];
        constexpr int maxPlayers{ 4 };
        const float increment{ sceneContext.windowWidth / (maxPlayers + 1) + 2 };

        if (i == 2)
        {
            // Add the timer where the third player would normally go
            m_pTimer = AddChild(new Timer(120, XMFLOAT2{ i * increment, 0.f }));

            // Then add the actual third player one position to the right
            AddChild(new CharacterPointDisplay(player, XMFLOAT2{ (i + 1) * increment, 0.f }));
        }
        else if (i > 2)
        {
            // For players after the third one, add them one position to the right
            AddChild(new CharacterPointDisplay(player, XMFLOAT2{ (i + 1) * increment, 0.f }));
        }
        else
        {
            // For the first two players, add them normally
            AddChild(new CharacterPointDisplay(player, XMFLOAT2{ i * increment, 0.f }));
            m_pTimer = AddChild(new Timer(120, XMFLOAT2{ i * increment, 0.f }));
        }
    }
}

void UIManager::ZeroTimer() const
{
    m_pTimer->ZeroTimer();
}


void UIManager::ResetTimer() const
{
    m_pTimer->ResetTimer();
}

void UIManager::StartTimer() const
{
    m_pTimer->StartTimer();
}

void UIManager::Update(const SceneContext&)
{
	
}