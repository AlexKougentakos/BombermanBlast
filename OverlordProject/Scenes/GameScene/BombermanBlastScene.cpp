#include "stdafx.h"
#include "BombermanBlastScene.h"

#include "Components/Grid.h"
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/RockPrefab.h"
#include "Components/PowerUpManager.h"
#include "Components/GameObjectManager.h"

#include <Windows.h>

#include "Materials/Post/Vignette.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/GameLoopManager.h"
#include "Prefabs/SkullBox.h"
#include "Prefabs/UI/UIManager.h"

BombermanBlastScene::BombermanBlastScene() :
	GameScene(L"BombermanBlastScene") {}


BombermanBlastScene::~BombermanBlastScene()
{
}

void BombermanBlastScene::Initialize()
{
	//auto& physx = PxGetPhysics();

	// SceneSettings
	m_SceneContext.settings.clearColor = XMFLOAT4{ 0,0,0,1 };
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
	
	//Fixed Camera
	m_pMainCamera = new FixedCamera();
	m_pMainCamera->GetTransform()->Translate(0, 85.f, -40.f);
	m_pMainCamera->GetTransform()->Rotate(66, 0, 0);
	AddChild(m_pMainCamera);
	SetActiveCamera(m_pMainCamera->GetComponent<CameraComponent>());
	m_pMainCamera->GetComponent<CameraComponent>()->SetOrthoSize(25);

	m_pDebugCamera = new FreeCamera();
	m_pDebugCamera->GetTransform()->Translate(0, 8.5f, -6.5f);
	m_pDebugCamera->GetTransform()->Rotate(60, 0, 0);
	AddChild(m_pDebugCamera);

	//Light
	m_SceneContext.pLights->SetDirectionalLight({ 0,300,0 }, { 0, -1, -0.01f });

	m_MapBottomLeft = { -43.f,0,-36.443f };
	m_MapTopRight = { 42.75f,0,33.f };

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.9f, 0.9f, 0.01f);
	//Level
	InitializeLevel(pDefaultMaterial);

	CalculateNeededBlockSize();

	m_pGrid = m_pLevel->AddComponent(new GridComponent(m_Map, m_MapBottomLeft, m_MapTopRight, m_SingleBlockSize, m_SingleBlockScale));
	m_pLevel->AddComponent(new PowerUpManager(m_pGrid));
	m_pLevel->AddComponent(new GameObjectManager());
	m_pLevel->GetTransform()->Scale({ 0.33f, 0.33f, 0.33f });
	
	DefinePlayerInputs();
	
	AddCharacters(pDefaultMaterial, m_NumOfPlayers);

	InitializeLevelNecessities();

	SpawnRocks();

	//Post Processing
	const auto vignette = MaterialManager::Get()->CreateMaterial<Vignette>();
	AddPostProcessingEffect(vignette);
}


void BombermanBlastScene::AddCharacters(PxMaterial* const pDefaultMaterial, const int numOfPlayers)
{
	CharacterDesc characterDesc{ pDefaultMaterial, 3.f, 10.f };

	m_pCharacters.clear();
	for (int i{}; i < numOfPlayers; ++i)
	{
		//Character
		int placementRow{}, placementColumn{};
		switch (i + 1)
		{
		case 1:
			//Bottom left
			placementRow = 1;
			placementColumn = 1;

			characterDesc.actionId_MoveForward = CharacterMoveForward_P1;
			characterDesc.actionId_MoveBackward = CharacterMoveBackward_P1;
			characterDesc.actionId_MoveLeft = CharacterMoveLeft_P1;
			characterDesc.actionId_MoveRight = CharacterMoveRight_P1;
			characterDesc.actionId_PlaceBomb = CharacterPlaceBomb_P1;
			characterDesc.actionId_Pause = PauseGame;
			break;
		case 2:
			//Bottom right
			placementRow = 1;
			placementColumn = m_NumOfColumns;

			characterDesc.actionId_MoveForward = CharacterMoveForward_P2;
			characterDesc.actionId_MoveBackward = CharacterMoveBackward_P2;
			characterDesc.actionId_MoveLeft = CharacterMoveLeft_P2;
			characterDesc.actionId_MoveRight = CharacterMoveRight_P2;
			characterDesc.actionId_PlaceBomb = CharacterPlaceBomb_P2;
			break;
		case 3:
			//Top right
			placementRow = m_NumOfRows;
			placementColumn = m_NumOfColumns;

			characterDesc.actionId_MoveForward = CharacterMoveForward_P3;
			characterDesc.actionId_MoveBackward = CharacterMoveBackward_P3;
			characterDesc.actionId_MoveLeft = CharacterMoveLeft_P3;
			characterDesc.actionId_MoveRight = CharacterMoveRight_P3;
			characterDesc.actionId_PlaceBomb = CharacterPlaceBomb_P3;
			break;
		case 4:
			//Top Left
			placementRow = m_NumOfRows;
			placementColumn = 1;

			characterDesc.actionId_MoveForward = CharacterMoveForward_P4;
			characterDesc.actionId_MoveBackward = CharacterMoveBackward_P4;
			characterDesc.actionId_MoveLeft = CharacterMoveLeft_P4;
			characterDesc.actionId_MoveRight = CharacterMoveRight_P4;
			characterDesc.actionId_PlaceBomb = CharacterPlaceBomb_P4;
			break;
		default:
			assert("Invalid player number");
			break;
		}
		characterDesc.maxMoveSpeed = 15.f;
		characterDesc.moveAccelerationTime = 0.01f;

		const auto character = m_pLevel->GetComponent<GameObjectManager>()->CreateGameObject<BombermanCharacter>(characterDesc, m_pGrid, i + 1);
		m_pCharacters.emplace_back(character);
		m_pGrid->PlaceObject(character, placementRow, placementColumn);
		const auto characterTransform = character->GetTransform();

		characterTransform->Scale(0.33f);
		//Slight offset to make sure the character is on the ground, otherwise he clips through
		characterTransform->Translate(characterTransform->GetPosition().x , 10.f, characterTransform->GetPosition().z);
	}
}

void BombermanBlastScene::PostInitialize()
{

}

void BombermanBlastScene::InitializeLevel(const PxMaterial* const pDefaultMaterial)
{
	ShowCursor(true);

	m_Map.resize(m_NumOfRows * m_NumOfColumns);
	m_Map.assign(m_Map.size(), 'O');

	m_pLevel = new GameObject();
	AddChild(m_pLevel);
	const auto levelRigidBody = m_pLevel->AddComponent(new RigidBodyComponent(true));

	//Add collider to level
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/MapHitbox.ovpt");
	levelRigidBody->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 0.33f,0.33f,0.33f })), *pDefaultMaterial);
	levelRigidBody->SetCollisionGroup(CollisionGroup::Level);

	//Add model to the level
	const auto pDiffuseMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDiffuseMat2->SetDiffuseTexture(L"Textures/map001_f.png");
	const auto pModelCompo = m_pLevel->AddComponent(new ModelComponent(L"Meshes/NewLevel.ovm", false));
	pModelCompo->SetMaterial(pDiffuseMat2->GetMaterialId());
}

void BombermanBlastScene::DefinePlayerInputs() const
{
	InputAction inputAction{};
	//P1
	inputAction = InputAction(CharacterMoveLeft_P1, InputState::down, 'J');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveRight_P1, InputState::down, 'L');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveForward_P1, InputState::down, 'I');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveBackward_P1, InputState::down, 'K');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterPlaceBomb_P1, InputState::pressed, 'H', -1, XINPUT_GAMEPAD_A, GamepadIndex::playerOne);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(PauseGame, InputState::pressed, VK_ESCAPE, -1, XINPUT_GAMEPAD_START, GamepadIndex::playerOne);
	m_SceneContext.pInput->AddInputAction(inputAction);
	//P2
	inputAction = InputAction(CharacterMoveLeft_P2, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveRight_P2, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveForward_P2, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveBackward_P2, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterPlaceBomb_P2, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//P3
	inputAction = InputAction(CharacterMoveLeft_P3, InputState::down, VK_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveRight_P3, InputState::down, VK_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveForward_P3, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveBackward_P3, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterPlaceBomb_P3, InputState::pressed, VK_LSHIFT, -1, XINPUT_GAMEPAD_A, GamepadIndex::playerThree);
	m_SceneContext.pInput->AddInputAction(inputAction);
	//P4
	inputAction = InputAction(CharacterMoveLeft_P4, InputState::down, VK_NUMPAD1);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveRight_P4, InputState::down, VK_NUMPAD3);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveForward_P4, InputState::down, VK_NUMPAD5);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMoveBackward_P4, InputState::down, VK_NUMPAD2);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterPlaceBomb_P4, InputState::pressed, VK_NUMPAD4, -1, XINPUT_GAMEPAD_A, GamepadIndex::playerFour);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void BombermanBlastScene::InitializeLevelNecessities()
{	
	m_pGameLoopManager = AddChild(new GameLoopManager(m_pCharacters));
	m_pGameLoopManager->registerObserver(this);
	
	m_pUIManager = AddChild(new UIManager(m_pCharacters, m_pGameLoopManager));
}

void BombermanBlastScene::ResetLevel()
{
	m_pGrid->ClearGrid();
	m_pLevel->GetComponent<PowerUpManager>()->RemovePowerUps();
	
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.9f, 0.9f, 0.01f);
	AddCharacters(pDefaultMaterial, m_NumOfPlayers);

	int placementRow{}, placementColumn{};
	for (size_t i{}; i < m_pCharacters.size(); ++i)
	{
		switch (i + 1)
		{
		case 1:
			//Bottom left
			placementRow = 1;
			placementColumn = 1;
			break;
		case 2:
			//Bottom right
			placementRow = 1;
			placementColumn = m_NumOfColumns;

			break;
		case 3:
			//Top right
			placementRow = m_NumOfRows;
			placementColumn = m_NumOfColumns;
			break;
		case 4:
			//Top Left
			placementRow = m_NumOfRows;
			placementColumn = 1;
			break;
		default:
			assert("Invalid player number");
			break;
		}

		m_pGrid->MoveObject(m_pCharacters[i], placementRow, placementColumn);
	}
	
	SpawnRocks();
}

void BombermanBlastScene::OnNotify(GameLoopManager* /*source*/, const std::string& event)
{
	if (event == "Pre-Round Start")
	{
		if (m_IsGameOver)
		{
			ResetLevel();
			m_pUIManager->ResetTimer();
			m_pUIManager->HideRoundWinners();
			m_pUIManager->UpdatePlayers(m_pCharacters);
		}

		m_pUIManager->StartCountdown();
	}

	if (event == "Round Start")
	{
		m_pUIManager->StartTimer();
	}

	if (event == "Round-With-Skull-Boxes-Start")
	{
		m_StartedDropping = true;
	}

	if (event == "Post-Round Start")
	{
		m_StartedDropping = false;
		m_ElapsedDroppingTime = 0.f;
		m_CurrentIndex = 0;
		
		m_pGrid->DeleteAllObjectsWithTag(L"SkullBoxFalling");
		m_pGrid->DeleteAllObjectsWithTag(L"Bomb");
		m_pUIManager->ShowRoundWinners();
		
		m_pUIManager->ZeroTimer();
		m_IsGameOver = true;
	}
}

void BombermanBlastScene::SpawnRocks() const 
{
	const auto gameObjectManager = m_pLevel->GetComponent<GameObjectManager>();

	for (int currentRow = 1; currentRow <= m_NumOfRows; ++currentRow)
	{
		for (int currentCol = 1; currentCol <= m_NumOfColumns; ++currentCol)
		{
			const int index = (currentRow - 1) * m_NumOfColumns + (currentCol - 1);
			if (m_StartingLayout[index] == 'R')
			{				
				m_pGrid->PlaceObject(gameObjectManager->CreateGameObject<RockPrefab>(RockType::BREAKABLE, m_SingleBlockScale), currentRow, currentCol);
			}
			else if (m_StartingLayout[index] == 'S')
			{
				m_pGrid->PlaceObject(gameObjectManager->CreateGameObject<RockPrefab>(RockType::UNBREAKABLE, m_SingleBlockScale), currentRow, currentCol);
			}
		}
	}
}

void BombermanBlastScene::OnSceneActivated()
{
	ShowCursor(true);
	m_SceneContext.pInput->ForceMouseToCenter(true);

	m_pGameLoopManager->StartGame();
}


void BombermanBlastScene::CalculateNeededBlockSize()
{
	//Spawn a rock to get the size of it in world space
	const auto defaultRock = new RockPrefab(RockType::BREAKABLE, 1);
	AddChild(defaultRock);

	const float width = m_MapTopRight.x - m_MapBottomLeft.x;
	m_SingleBlockScale = width / (m_NumOfColumns * defaultRock->GetWidth());
	m_SingleBlockSize = defaultRock->GetWidth() * m_SingleBlockScale;

	RemoveChild(defaultRock, true);
}

void BombermanBlastScene::Update()
{
	//m_SceneContext.pInput->ForceMouseToCenter(true);
	
	for (const auto player : m_pCharacters)
	{

		if (!player || player->IsDead())
		{
			std::erase(m_pCharacters, player);
			m_pGrid->RemoveObject(player);
		}
	}

	if (m_StartedDropping)
	{
		m_ElapsedDroppingTime += m_SceneContext.pGameTime->GetElapsed();
		if (m_ElapsedDroppingTime >= m_BoxDropInterval)
		{
			const int currentRow = m_CurrentIndex / m_NumOfColumns;
			const int currentCol = m_CurrentIndex % m_NumOfColumns;
    
			if (currentRow < m_NumOfRows && currentCol < m_NumOfColumns)
			{
				if (m_pGrid->GetCell(currentRow + 1, currentCol + 1).Contains(L"UnbreakableRock"))
				{
					m_CurrentIndex++;
					return;
				}
				const auto pSkullBox = new SkullBox(m_SingleBlockScale, m_pGrid);
				m_pSkullBoxes.emplace_back(pSkullBox);
				m_pGrid->PlaceObject(pSkullBox, currentRow + 1, currentCol + 1);
				pSkullBox->PlacedInGrid();
				
				m_CurrentIndex++;
				m_ElapsedDroppingTime = 0.f;
			}
			else
			{
				m_StartedDropping = false;
				m_ElapsedDroppingTime = 0.f;
				m_CurrentIndex = 0;
			}
		}
	}
		

	m_pGrid->UpdateCharacterOnMap(m_pCharacters);
	m_pLevel->GetComponent<PowerUpManager>()->UpdatePowerUps();
}

void BombermanBlastScene::Draw()
{
}

void BombermanBlastScene::OnGUI()
{
	if (ImGui::Button("Toggle Camera"))
	{
		if (m_pMainCamera->GetComponent<CameraComponent>()->IsActive())
		{
			m_pMainCamera->GetComponent<CameraComponent>()->SetActive(false);
			m_pDebugCamera->GetComponent<CameraComponent>()->SetActive(true);
			SetActiveCamera(m_pDebugCamera->GetComponent<CameraComponent>());
			m_SceneContext.settings.inDebug = true;
		}
		else
		{
			m_pDebugCamera->GetComponent<CameraComponent>()->SetActive(false);
			m_pMainCamera->GetComponent<CameraComponent>()->SetActive(true);
			SetActiveCamera(m_pMainCamera->GetComponent<CameraComponent>());
			m_SceneContext.settings.inDebug = false;
		}
	}

	if (ImGui::Button("Add Object"))
	{
		m_pGrid->PlaceObject(new RockPrefab(RockType::BREAKABLE),
			m_pGrid->GetCellOnTop(1, 1));
		
	}
	
	for (const auto player : m_pCharacters)
	{
		player->DrawImGui();
	}
	
	m_pGameLoopManager->DrawOnGUI();

	//Display the map
	ImGui::Begin("Map Display");

	// Calculate the size of each button
	const ImVec2 buttonSize(30, 30);  // Adjust the size as per your requirements

	// Iterate over the m_map vector and display each element as a button
	for (int row = m_NumOfRows - 1; row >= 0; --row)  // Start from bottom-left
	{
		for (int col = 0; col < m_NumOfColumns; ++col)
		{
			// Calculate the index in the m_map vector based on row and column
			const int index = row * m_NumOfColumns + col;

			// Determine the color based on the char value
			ImVec4 buttonColor;
			switch (m_Map[index])
			{
			case 'R':
				buttonColor = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);  // Brownish color
				break;
			case 'S':
				buttonColor = ImVec4(0.8f, 0.6f, 0.4f, 1.0f); //Light brown 
				break;
			case 'P':
				buttonColor = ImVec4(0.86f, 0.08f, 0.24f, 1.0f);  // Crimson color
				break;
			case 'B':
				buttonColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // Dark gray (almost black)
				break;
			case 'O':
				buttonColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);  // Almost white
				break;
			case 'X':
				buttonColor = ImVec4(0.95f, 0.7f, 0.2f, 1.0f);  // Light Orange
				break;
			case 'U':
				buttonColor = ImVec4(0.15f, 0.0f, 0.7f, 1.0f);  // Light Blue
				break;
			default:
				buttonColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default color (white)
				break;
			}

			// Set the button color
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

			// Display the button
			ImGui::PushID(index);
			ImGui::Button(std::string(1, m_Map[index]).c_str(), buttonSize);

			// Check if the button is hovered
			if (ImGui::IsItemHovered())
			{
				// Display the tooltip with the currentIndex
				std::string tooltipText = "Current Index: " + std::to_string(index) + "\n";

				// Append the vector of objects as strings to the tooltip
				const auto& cellObjects = m_pGrid->GetCell(index).objects;
				tooltipText += "Cell Objects:\n";
				for (const auto& item : cellObjects)
				{
					if (item)
					{
						// Convert wide string to narrow string
						const std::wstring& wideStr = item->GetTag();

						const int narrowStrSize = WideCharToMultiByte(
							CP_UTF8,        // UTF-8 encoding
							0,              // Default flags
							wideStr.c_str(),
							static_cast<int>(wideStr.size()),
							nullptr,
							0,
							nullptr,
							nullptr
						);

						std::string narrowStr(narrowStrSize, '\0');

						WideCharToMultiByte(
							CP_UTF8,        // UTF-8 encoding
							0,              // Default flags
							wideStr.c_str(),
							static_cast<int>(wideStr.size()),
							narrowStr.data(),
							narrowStrSize,
							nullptr,
							nullptr
						);

						tooltipText += narrowStr + "\n";
					}
				}

				ImGui::SetTooltip("%s", tooltipText.c_str());
			}

			ImGui::PopID();
			ImGui::SameLine();  // Align buttons horizontally

			// Reset the button color
			ImGui::PopStyleColor();
		}
		ImGui::NewLine();  // Move to the next row
	}

	// End ImGui window
	ImGui::End();
}