#include "stdafx.h"
#include "MainMenuScene.h"
#include "Prefabs/UI/Menu/ButtonManager.h"
#include "Prefabs/UI/Menu/Buttons/StartButton.h"
#include "Prefabs/UI/Menu/Buttons/ControllsButton.h"
#include "Prefabs/UI/Menu/Buttons/QuitButton.h"


void MainMenuScene::Initialize()
{
	m_pButtonManager = new ButtonManager();
	AddChild(m_pButtonManager);

	const auto fixedCam = new FixedCamera();
	AddChild(fixedCam);
	SetActiveCamera(fixedCam->GetComponent<CameraComponent>());

	const auto background = new GameObject();
	AddChild(background);
	background->AddComponent(new SpriteComponent(L"Textures/UI/Background.png"));
	const float scale = m_SceneContext.windowWidth / background->GetComponent<SpriteComponent>()->GetDimensions().x;
	background->GetTransform()->Scale(scale);
	background->GetTransform()->Translate(0, 0, .9f);

	m_pCursor = new GameObject();
	AddChild(m_pCursor);

	m_pCursor->AddComponent(new SpriteComponent(L"Textures/UI/Cursor.png"));
	m_pCursor->GetTransform()->Scale(0.25f);

	const XMFLOAT2 startButtonPos{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 1.5f - 125 };
	const auto startButton = new StartButton(startButtonPos, L"Textures/UI/StartButton.png");
	AddChild(startButton);

	const XMFLOAT2 controlsButtonPos{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 1.5f };
	const auto controlsButton = new ControllButton(controlsButtonPos, L"Textures/UI/SettingsButton.png");
	AddChild(controlsButton);

	const XMFLOAT2 quitButtonPos{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 1.5f + 125 };
	const auto quitButton = new QuitButton(quitButtonPos, L"Textures/UI/QuitButton.png");
	AddChild(quitButton);

	m_pButtonManager->AddButton(startButton);
	m_pButtonManager->AddButton(controlsButton);
	m_pButtonManager->AddButton(quitButton);

	//Hide the mouse cursor
	ShowCursor(FALSE);

	//todo: test this pInput->ForceMouseToCenter(true)

}

void MainMenuScene::Update()
{
	UpdateCustomCursor();
}

void MainMenuScene::UpdateCustomCursor()
{
	//const XMFLOAT2 mousePos = XMFLOAT2{ static_cast<float>(InputManager::GetMousePosition().x),
	//	static_cast<float>(InputManager::GetMousePosition().y) };

	//const float halfWidth = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().x / 2.f;
	//const float halfHeight = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().y / 2.f;

	//if (InputManager::GetMouseMovementNormalized().x != 0 || InputManager::GetMouseMovementNormalized().y != 0)
	//	m_GameCursorPosition = mousePos;
	//	

	//constexpr float controllerMouseMoveSpeed{500.f};

	//m_GameCursorPosition.x += InputManager::GetThumbstickPosition().x * controllerMouseMoveSpeed * m_SceneContext.pGameTime->GetElapsed();
	//m_GameCursorPosition.y -= InputManager::GetThumbstickPosition().y * controllerMouseMoveSpeed * m_SceneContext.pGameTime->GetElapsed();

	//m_GameCursorPosition.x = std::clamp(m_GameCursorPosition.x, halfWidth, m_SceneContext.windowWidth - halfWidth);
	//m_GameCursorPosition.y = std::clamp(m_GameCursorPosition.y, halfHeight, m_SceneContext.windowHeight - halfHeight);

	//m_pCursor->GetTransform()->Translate(m_GameCursorPosition.x - halfWidth, m_GameCursorPosition.y - halfHeight , 0.f);

	const XMFLOAT2 mousePos = XMFLOAT2{ static_cast<float>(InputManager::GetMousePosition().x),
									   static_cast<float>(InputManager::GetMousePosition().y) };

	if (InputManager::GetMouseMovementNormalized().x != 0 || InputManager::GetMouseMovementNormalized().y != 0)
		m_GameCursorPosition = mousePos;

	constexpr float controllerMouseMoveSpeed{ 500.f };

	m_GameCursorPosition.x += InputManager::GetThumbstickPosition().x * controllerMouseMoveSpeed * m_SceneContext.pGameTime->GetElapsed();
	m_GameCursorPosition.y -= InputManager::GetThumbstickPosition().y * controllerMouseMoveSpeed * m_SceneContext.pGameTime->GetElapsed();

	// Clamp values to the screen boundaries
	const float width = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().x;
	const float height = m_pCursor->GetComponent<SpriteComponent>()->GetDimensions().y;

	m_GameCursorPosition.x = std::clamp(m_GameCursorPosition.x, 0.f, m_SceneContext.windowWidth - width);
	m_GameCursorPosition.y = std::clamp(m_GameCursorPosition.y, 0.f, m_SceneContext.windowHeight - height);

	constexpr float smallOffset{8.f}; //To match up the cursor tip with the actual mouse position
	m_pCursor->GetTransform()->Translate(m_GameCursorPosition.x - smallOffset, m_GameCursorPosition.y - smallOffset, 0.f);
}
