#include "stdafx.h"
#include "UIManager.h"
#include "CharacterPointDisplay.h"
#include "CountDown.h"
#include "Timer.h"

UIManager::UIManager(std::vector<BombermanCharacter*> players)
	:m_pPlayers(players)
{

}

void UIManager::Initialize(const SceneContext& sceneContext)
{
    for (size_t i = 0; i < m_pPlayers.size(); ++i) 
    {
        const auto player = m_pPlayers[i];
        const float segmentWidth = sceneContext.windowWidth / 5.0f;

        // Calculate position based on player index, adjusting for the middle timer slot.
        size_t adjustedIndex = i;
        if (m_pPlayers.size() == 2) 
        {
            // For two players, place the second player in the last segment.
            adjustedIndex = i == 0 ? 0 : 4; // First player at 0, second player at 4.
        }
        else if (i >= 2) 
        {
            // For more than two players, skip the middle slot for players 3 and 4.
            adjustedIndex++;
        }

        XMFLOAT2 playerPosition = XMFLOAT2(adjustedIndex * segmentWidth + segmentWidth / 2, 0.f); // Center in segment.
        if (i == 0)
            playerPosition = XMFLOAT2(adjustedIndex * segmentWidth, 0.f); // Keep the first player at the far left edge.

        AddChild(new CharacterPointDisplay(player, playerPosition));
    }

    // Place the timer in the middle segment, which is the 3rd spot (index 2 in 0-based indexing).
    const XMFLOAT2 timerPosition = XMFLOAT2(2 * sceneContext.windowWidth / 5.0f + sceneContext.windowWidth / 10.0f, 0.f); // Centered in the 3rd segment.
    m_pTimer = AddChild(new Timer(120, timerPosition));

    m_pCountdown = AddChild(new CountDown(5, 
        XMFLOAT2(sceneContext.windowWidth / 2, sceneContext.windowHeight / 2)));

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

void UIManager::StartCountdown() const
{
    m_pCountdown->StartCountDown();
}