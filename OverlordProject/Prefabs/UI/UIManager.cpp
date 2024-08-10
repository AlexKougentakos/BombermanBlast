#include "stdafx.h"
#include "UIManager.h"
#include "CharacterPointDisplay.h"
#include "CountDown.h"
#include "Timer.h"
#include "Menu/ButtonManager.h"
#include "Menu/Buttons/BackButton.h"
#include "Menu/Buttons/ContinueButton.h"
#include "Menu/Buttons/QuitButton.h"
#include "Prefabs/BombermanCharacter.h"

UIManager::UIManager(std::vector<BombermanCharacter*> players, GameLoopManager* pGameLoopManager)
	:m_pPlayers(players),
    m_pGameLoopManager(pGameLoopManager)
{
    for (const auto player : m_pPlayers)
    {
        player->registerObserver(this);
    }
}

void UIManager::Initialize(const SceneContext& sceneContext)
{
    AddPlayerHeads(sceneContext);
    AddTimer(sceneContext);
    

    m_StartButtonPos = { sceneContext.windowWidth / 2.f, sceneContext.windowHeight / 1.5f - 125 };
    m_ContinueButtonPos = { sceneContext.windowWidth / 2.f, sceneContext.windowHeight / 1.5f };
}

void UIManager::OnNotify(BombermanCharacter*, const std::string& field)
{
    if (field == "Game Pause")
    {
        ShowPauseMenu();
        m_ShowingPauseMenu = true;
    }
    else if (field == "Game UnPause")
    {
        m_ShowingPauseMenu = false;
        RemoveChild(m_pPauseMenuContainer, true);
        m_pCountdown->SetActive(true);
    }
}

void UIManager::Update(const SceneContext& sceneContext)
{
    if (!m_ShowingPauseMenu) return;
    
    const XMFLOAT2 mousePos = XMFLOAT2{ static_cast<float>(InputManager::GetMousePosition().x),
                                   static_cast<float>(InputManager::GetMousePosition().y) };

    if (InputManager::GetMouseMovementNormalized().x != 0 || InputManager::GetMouseMovementNormalized().y != 0)
        m_GameCursorPosition = mousePos;

    constexpr float controllerMouseMoveSpeed{ 500.f };

    std::cout << InputManager::GetThumbstickPosition().x << " " << InputManager::GetThumbstickPosition().y << std::endl;
    m_GameCursorPosition.x += InputManager::GetThumbstickPosition().x * controllerMouseMoveSpeed * sceneContext.pGameTime->GetElapsedUnpaused();
    m_GameCursorPosition.y -= InputManager::GetThumbstickPosition().y * controllerMouseMoveSpeed * sceneContext.pGameTime->GetElapsedUnpaused();

    // Clamp values to the screen boundaries
    const float width = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().x;
    const float height = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().y;

    m_GameCursorPosition.x = std::clamp(m_GameCursorPosition.x, 0.f, sceneContext.windowWidth - width);
    m_GameCursorPosition.y = std::clamp(m_GameCursorPosition.y, 0.f, sceneContext.windowHeight - height);

    constexpr float smallOffset{8.f}; //To match up the cursor tip with the actual mouse position
    m_pCursor->GetTransform()->Translate(m_GameCursorPosition.x - smallOffset, m_GameCursorPosition.y - smallOffset, 0.f);
}

void UIManager::ShowPauseMenu()
{
    m_pPauseMenuContainer = new GameObject();
    AddChild(m_pPauseMenuContainer);
    
    m_pButtonManager = new ButtonManager(&m_GameCursorPosition);
    m_pPauseMenuContainer->AddChild(m_pButtonManager);
        
    m_pCursor = new GameObject();
    m_pPauseMenuContainer->AddChild(m_pCursor);
    m_pCursor->AddComponent(new SpriteComponent(L"Textures/UI/Cursor.png"));
    m_pCursor->GetTransform()->Scale(0.25f);
    const XMFLOAT2 mousePos = XMFLOAT2{ static_cast<float>(InputManager::GetMousePosition().x),
                                   static_cast<float>(InputManager::GetMousePosition().y) };

    const auto continueButton = new QuitButton(m_ContinueButtonPos, L"Textures/UI/PlayButton.png");
    const auto startButton = new BackButton(m_StartButtonPos, L"Textures/UI/QuitButton.png");

    m_pButtonManager->AddButton(startButton);
    m_pButtonManager->AddButton(continueButton);

    m_pCountdown->SetActive(false);
}

void UIManager::AddPlayerHeads(const SceneContext& sceneContext)
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

        XMFLOAT2 screenPosition = XMFLOAT2(adjustedIndex * segmentWidth + segmentWidth / 2, 0.f); // Center in segment.
        if (i == 0)
            screenPosition = XMFLOAT2(adjustedIndex * segmentWidth, 0.f); // Keep the first player at the far left edge.
        
        AddChild(new CharacterPointDisplay(player->GetIndex(), m_pGameLoopManager, screenPosition));
    }
}

void UIManager::AddTimer(const SceneContext& sceneContext)
{
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

void UIManager::StartCountdown() const
{
    m_pCountdown->StartCountDown();
}

void UIManager::UpdatePlayers(const std::vector<BombermanCharacter*>& players)
{
    m_pPlayers = players;
    for (const auto player : m_pPlayers)
    {
        player->registerObserver(this);
    }
}
