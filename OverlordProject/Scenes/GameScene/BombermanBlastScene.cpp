#include "stdafx.h"
#include "BombermanBlastScene.h"

#include "Components/Grid.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/BombPrefab.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/RockPrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/PowerUps/PowerUp_IncrDecrBlast.h"
#include "Components/PowerUpManager.h"

#include <Windows.h>

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/UI/UIManager.h"


BombermanBlastScene::BombermanBlastScene() :
	GameScene(L"BombermanBlastScene") {}


void BombermanBlastScene::Initialize()
{
	//auto& physx = PxGetPhysics();

	// SceneSettings
	m_SceneContext.settings.clearColor = XMFLOAT4{ 0,0,0,1 };
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.inDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	//Fixed Camera
	m_pMainCamera = new FixedCamera();
	m_pMainCamera->GetTransform()->Translate(0, 85.f, -65.f);
	m_pMainCamera->GetTransform()->Rotate(60, 0, 0);
	AddChild(m_pMainCamera);
	SetActiveCamera(m_pMainCamera->GetComponent<CameraComponent>());
	m_pMainCamera->GetComponent<CameraComponent>()->SetOrthoSize(25);

	m_pDebugCamera = new FreeCamera();
	m_pDebugCamera->GetTransform()->Translate(0, 8.5f, -6.5f);
	m_pDebugCamera->GetTransform()->Rotate(60, 0, 0);
	AddChild(m_pDebugCamera);

	//Light
	m_SceneContext.pLights->SetDirectionalLight({ 0,20,0 }, {0, -1, 0.001f});

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.9f, 0.9f, 0.5f);

	m_MapBottomLeft = { -43.f,0,-35.443f };
	m_MapTopRight = { 49.9f,0,34.f };

	//Level
	//Fill the map with 'O' for open space
	m_Map.resize(m_NumOfRows * m_NumOfColumns);
	m_Map.assign(m_Map.size(), 'O');

	m_pLevel = new GameObject();
	AddChild(m_pLevel);
	const auto levelRigidBody = m_pLevel->AddComponent(new RigidBodyComponent(true));
	m_pLevel->GetTransform()->Scale(10);

	//Add collider to level
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Level.ovpt");
	levelRigidBody->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 10.f,10.f,10.f })), *pDefaultMaterial);
	levelRigidBody->SetCollisionGroup(CollisionGroup::Group2);
	levelRigidBody->GetPxRigidActor()->setName("level");

	//Add model to the level
	const auto pDiffuseMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDiffuseMat2->SetDiffuseTexture(L"Textures/7EE07A6D_c.png");
	const auto pModelCompo = m_pLevel->AddComponent(new ModelComponent(L"Meshes/Level.ovm", false));
	pModelCompo->SetMaterial(pDiffuseMat2->GetMaterialId());

	CalculateNeededBlockSize();

	//Add grid component to level
	m_pLevel->AddComponent(new GridComponent(m_Map, m_MapBottomLeft, m_MapTopRight, m_SingleBlockSize, m_SingleBlockScale));
	m_pLevel->AddComponent(new PowerUpManager(m_pLevel->GetComponent<GridComponent>()));

	InputAction inputAction{};
	//P1
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
	//P2
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
	//todo: add more support for player input ^^^

	constexpr unsigned int numOfPlayers{ 2 };
	for (int i{}; i < numOfPlayers; ++i)
	{
		//Character
		CharacterDesc characterDesc{ pDefaultMaterial, 3.f, 10.f };
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
			placementColumn = 0;

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


		const auto character = new BombermanCharacter(characterDesc, m_pLevel->GetComponent<GridComponent>());
		m_pCharacters.emplace_back(character);
		m_pLevel->GetComponent<GridComponent>()->PlaceObject(character, placementRow, placementColumn);
		const auto characterTransform = character->GetTransform();

		characterTransform->Scale(10.f);
		//Slight offset to make sure the character is on the ground, otherwise he clips through
		characterTransform->Translate(characterTransform->GetPosition().x , 10.f, characterTransform->GetPosition().z);
	}

	SpawnRocks();

	AddChild(new UIManager(m_pCharacters));

	auto go = new GameObject();
	AddChild(go);
	go->AddComponent(new SpriteComponent(L"Textures/UI/transparent.png"));
}

void BombermanBlastScene::SpawnRocks() const 
{
	const auto grid = m_pLevel->GetComponent<GridComponent>();

	for (int currentRow = 1; currentRow <= m_NumOfRows; ++currentRow)
	{
		for (int currentCol = 1; currentCol <= m_NumOfColumns; ++currentCol)
		{
			const int index = (currentRow - 1) * m_NumOfColumns + (currentCol - 1);
			if (m_StartingLayout[index] == 'R')
			{
				grid->PlaceObject(new RockPrefab(RockType::BREAKABLE, m_SingleBlockScale), currentRow, currentCol);
			}
		}
	}
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
	for (const auto player : m_pCharacters)
		if (!player)
			std::erase(m_pCharacters, player);

	m_pLevel->GetComponent<GridComponent>()->UpdateCharacterOnMap(m_pCharacters);
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
		m_pLevel->GetComponent<GridComponent>()->PlaceObject(new RockPrefab(RockType::BREAKABLE),
			m_pLevel->GetComponent<GridComponent>()->GetCellOnTop(1, 1));
		
	}

	for (const auto player : m_pCharacters)
	{
		player->DrawImGui();
	}

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
				const auto grid = m_pLevel->GetComponent<GridComponent>();
				const auto& cellObjects = grid->GetCell(index).objects;
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