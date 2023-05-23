#include "stdafx.h"
#include "BombermanBlastScene.h"

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

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 0.2f, 0.2f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new BombermanCharacter(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);

	//Add Model to Character
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();

	m_pModelComponent = m_pCharacter->AddComponent(new ModelComponent(L"Meshes/Character.ovm"));
	pDiffuseMat->SetDiffuseTexture(L"Textures/WhiteBomberMan.png");
	m_pModelComponent->SetMaterial(pDiffuseMat->GetMaterialId());
	m_pModelComponent->GetTransform()->Translate(0, 10.f, 0);

	m_pModelAnimator = m_pModelComponent->GetAnimator();
	m_pModelAnimator->SetAnimation(m_AnimationClipId);
	m_pModelAnimator->SetAnimationSpeed(1.f);
	m_pModelAnimator->Play();

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);
	
	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Level
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

	//Add stones to level
	m_MapBottomLeft = { -4.3f,0,-3.55f };
	m_MapTopRight = { 5.0f,0,3.45f };

	SpherePrefab* pBottomLeftPoint{new SpherePrefab(0.2f)}, * pTopRightPoint{new SpherePrefab(0.2f)};
	AddChild(pBottomLeftPoint);
	AddChild(pTopRightPoint);
		
	pBottomLeftPoint->GetTransform()->Translate(m_MapBottomLeft);
	pTopRightPoint->GetTransform()->Translate(m_MapTopRight);

	SpawnRocks();

	test = new BombPrefab();
	test->GetTransform()->Translate(1.f, 0.f, 0.f);
	AddChild(test);

}

void BombermanBlastScene::SpawnRocks()
{
	//Make sure these align with the map
	constexpr int numRows{ 10 }, numColumns{ 13 };
	constexpr float offset{ 0.f };

	const auto defaultRock = new RockPrefab(RockType::BREAKABLE, 1);
	AddChild(defaultRock);

	const float width = m_MapTopRight.x - m_MapBottomLeft.x;
	const float scale = width / (numColumns * defaultRock->GetWidth());
	const float newWidth = defaultRock->GetWidth() * scale;

	//Update the block size
	m_SingleBlockSize = newWidth;

	for (int currentRow{ 0 }; currentRow < numRows; ++currentRow)
	{
		for (int currentCol{ 0 }; currentCol < numColumns; ++currentCol)
		{
			// Get the value at the corresponding index in m_Map
			const char mapValue = m_Map[currentRow * numColumns + currentCol];

			if (mapValue == 'X')
			{
				const auto pRock = new RockPrefab(RockType::BREAKABLE, scale);
				AddChild(pRock);

				const auto pos = XMFLOAT3{
					m_MapBottomLeft.x + newWidth / 2 + currentCol * newWidth + offset,
					0,
					m_MapBottomLeft.z + newWidth / 2 + currentRow * newWidth + offset
				};	

				pRock->GetTransform()->Translate(pos);
			}
		}
	}

	RemoveChild(defaultRock, true);
}

void BombermanBlastScene::ExplodeBomb(const BombPrefab& bomb)
{
	const auto origin = PhysxHelper::ToPxVec3(bomb.GetTransform()->GetPosition());
	const auto direction = PxVec3{ 0,0,1 };
	
	std::vector<RockPrefab*> hitRocks{};

	PxRaycastBuffer hit{};
	this->GetPhysxProxy()->Raycast(origin, direction, m_SingleBlockSize, hit);

	for (PxU32 i = 0; i < hit.nbTouches; i++)
	{
		if (hit.touches[i].actor->userData != nullptr)
		{
			RockPrefab* rock = static_cast<RockPrefab*>(hit.touches[i].actor->userData);
			if (rock != nullptr)
			{
				hitRocks.push_back(rock);
			}
		}
	}

	for(auto rock : hitRocks)
	{
		RemoveChild(rock, true);
	}
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
	//Optional
}

void BombermanBlastScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);

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

	if (ImGui::Button("Explode Bomb"))
		ExplodeBomb(*test);

}
