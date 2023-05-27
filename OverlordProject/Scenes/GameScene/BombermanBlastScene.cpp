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
	m_pMainCamera->GetTransform()->Translate(0, 8.5f, -6.5f);
	m_pMainCamera->GetTransform()->Rotate(60, 0, 0);
	AddChild(m_pMainCamera);
	SetActiveCamera(m_pMainCamera->GetComponent<CameraComponent>());

	m_pDebugCamera = new FreeCamera();
	m_pDebugCamera->GetTransform()->Translate(0, 8.5f, -6.5f);
	m_pDebugCamera->GetTransform()->Rotate(60, 0, 0);
	AddChild(m_pDebugCamera);

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.9f, 0.9f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);
	
	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterPlaceBomb, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	m_MapBottomLeft = { -4.3f,0,-3.55f };
	m_MapTopRight = { 5.0f,0,3.45f };

	//Level
	//Fill the map with 'O' for open space
	m_Map.resize(m_NumOfRows * m_NumOfColumns);
	m_Map.assign(m_Map.size(), 'O');

	m_pLevel = new GameObject();
	AddChild(m_pLevel);
	const auto levelRigidBody = m_pLevel->AddComponent(new RigidBodyComponent(true));

	//Add collider to level
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Level.ovpt");
	levelRigidBody->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 1.f,1.f,1.f })), *pDefaultMaterial);

	//Add model to the level
	const auto pDiffuseMat2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pDiffuseMat2->SetDiffuseTexture(L"Textures/7EE07A6D_c.png");
	const auto pModelCompo = m_pLevel->AddComponent(new ModelComponent(L"Meshes/Level.ovm"));
	pModelCompo->SetMaterial(pDiffuseMat2->GetMaterialId());

	//Define Level Boundries
	SpherePrefab* pBottomLeftPoint{new SpherePrefab(0.2f)}, * pTopRightPoint{new SpherePrefab(0.2f)};
	AddChild(pBottomLeftPoint);
	AddChild(pTopRightPoint);
		
	pBottomLeftPoint->GetTransform()->Translate(m_MapBottomLeft);
	pTopRightPoint->GetTransform()->Translate(m_MapTopRight);

	CalculateNeededBlockSize();

	//Add grid component to level
	m_pLevel->AddComponent(new GridComponent(m_Map, m_MapBottomLeft, m_MapTopRight, m_SingleBlockSize));
	
	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 0.35f, 1.f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_PlaceBomb = CharacterPlaceBomb;

	m_pCharacter = AddChild(new BombermanCharacter(characterDesc, m_pLevel->GetComponent<GridComponent>()));

	const auto pModelForCharacter = new GameObject();
	m_pCharacter->AddChild(pModelForCharacter);
	pModelForCharacter->GetTransform()->Translate(0, -characterDesc.controller.height + characterDesc.controller.radius / 2.f, 0);
	m_pCharacter->GetTransform()->Translate(0, 10, 0);
	//Add Model to Character
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();

	m_pModelComponent = pModelForCharacter->AddComponent(new ModelComponent(L"Meshes/Character.ovm"));
	pDiffuseMat->SetDiffuseTexture(L"Textures/WhiteBomberMan.png");
	m_pModelComponent->SetMaterial(pDiffuseMat->GetMaterialId());

	m_pModelAnimator = m_pModelComponent->GetAnimator();
	m_pModelAnimator->SetAnimation(m_AnimationClipId);
	m_pModelAnimator->SetAnimationSpeed(1.f);
	m_pModelAnimator->Play();

	SpawnRocks();
}

void BombermanBlastScene::SpawnRocks() const 
{
	const auto grid = m_pLevel->GetComponent<GridComponent>();

	//Todo: redo this to work with the grid
	for (int currentRow = 1; currentRow <= m_NumOfRows; ++currentRow)
	{
		for (int currentCol = 1; currentCol <= m_NumOfColumns; ++currentCol)
		{
			const int index = (currentRow - 1) * m_NumOfColumns + (currentCol - 1);
			if (m_StartingLayout[index] == 'R')
			{
				grid->PlaceObject(new RockPrefab(RockType::BREAKABLE), currentRow, currentCol);
			}
		}
	}
}

void BombermanBlastScene::CalculateNeededBlockSize()
{
	const auto defaultRock = new RockPrefab(RockType::BREAKABLE, 1);
	AddChild(defaultRock);

	const float width = m_MapTopRight.x - m_MapBottomLeft.x;
	m_SingleBlockScale = width / (m_NumOfColumns * defaultRock->GetWidth());
	m_SingleBlockSize = defaultRock->GetWidth() * m_SingleBlockScale;

	RemoveChild(defaultRock, true);
}

void BombermanBlastScene::Update()
{
	//Update the Direction of the Model
	m_pModelComponent->GetTransform()->Rotate( 0, m_pCharacter->GetYAxisRotation(), 0);
	//if (m_SceneContext.pInput->IsActionTriggered(CharacterMoveForward)
	//	|| m_SceneContext.pInput->IsActionTriggered(CharacterMoveBackward) ||
	//	m_SceneContext.pInput->IsActionTriggered(CharacterMoveRight) ||
	//	m_SceneContext.pInput->IsActionTriggered(CharacterMoveLeft))
	//{
	//	//TODO: set animation clip correctly here
	//	ChangeAnimationClip(1);
	//}
	//else
	//	ChangeAnimationClip(0);
	//ExplodeBomb(*test);
	m_pLevel->GetComponent<GridComponent>()->UpdateCharacterOnMap(m_pCharacter->GetTransform()->GetPosition());
}

void BombermanBlastScene::ChangeAnimationClip(UINT animationID)
{
	if (m_AnimationClipId != animationID)
	{
		m_AnimationClipId = animationID;
		m_pModelAnimator->SetAnimation(m_AnimationClipId);
		m_pModelAnimator->Play();
	}
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

	//Display the map
	ImGui::Begin("Map Display");

	static int selectedCell{ 0 };
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
				ImGui::SetTooltip("Current Index: %d", index);
			}

			ImGui::PopID();
			ImGui::SameLine();  // Align buttons horizontally

			// Reset the button color
			ImGui::PopStyleColor();
		}
		ImGui::NewLine();  // Move to the next row
	}


	if (ImGui::CollapsingHeader("Cell Contents", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputInt("SetValue", &selectedCell);

		const auto grid = m_pLevel->GetComponent<GridComponent>();
		for (const auto item : grid->GetCell(selectedCell).objects)
		{
			if (item)
				ImGui::Text("%s", item->GetTag().c_str());
		}
	}

	// End ImGui window
	ImGui::End();
}