#include "stdafx.h"
#include "MainMenuScene.h"
#include "Prefabs/UI/Menu/ButtonManager.h"
#include "Prefabs/UI/Menu/Buttons/StartButton.h"
#include "Prefabs/UI/Menu/Buttons/ControllsButton.h"
#include "Prefabs/UI/Menu/Buttons/QuitButton.h"

MainMenuScene::MainMenuScene() : GameScene(L"MainMenuScene")
{
}


void MainMenuScene::Initialize()
{
#ifdef _DEBUG
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.enableOnGUI = true;
#else
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.showInfoOverlay = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.inDebug = false;
	m_SceneContext.settings.enableOnGUI = false;
#endif
	
	
	m_pButtonManager = new ButtonManager(&m_GameCursorPosition);
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

	const XMFLOAT2 controlsButtonPos{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 1.5f };
	const auto controlsButton = new ControllButton(controlsButtonPos, L"Textures/UI/SettingsButton.png");

	const XMFLOAT2 quitButtonPos{ m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 1.5f + 125 };
	const auto quitButton = new QuitButton(quitButtonPos, L"Textures/UI/QuitButton.png");

	m_pButtonManager->AddButton(startButton);
	m_pButtonManager->AddButton(controlsButton);
	m_pButtonManager->AddButton(quitButton);

	//todo: test this pInput->ForceMouseToCenter(true)
}

void MainMenuScene::OnSceneActivated()
{
#ifndef _DEBUG
	ShowCursor(FALSE);
#endif 
	m_SceneContext.pInput->SetEnabled(true);
}

void MainMenuScene::Update()
{
	UpdateCustomCursor();
}

void MainMenuScene::UpdateCustomCursor()
{
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
