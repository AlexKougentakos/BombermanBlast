#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"


PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	auto& physx = PxGetPhysics();


	// SceneSettings
	m_SceneContext.settings.clearColor = XMFLOAT4{ 0,0,0,1 };
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	//Logger::LogInfo(L"Welcome, humble Minion!");

	m_pBall = new SpherePrefab();
	m_pPaddleLeft = new CubePrefab(2, 5, 2);
	m_pPaddleRight = new CubePrefab(2, 5, 2);

	const auto pDefaultMat = physx.createMaterial(.0f, .0f, 1.f);

	// Level
	{
		constexpr XMFLOAT3 dimensions{ 60,3,3 };
		constexpr float displacement{ 15.f };
		const auto pCeiling = new GameObject();
		

		AddChild(pCeiling);
		auto pRigidBody = pCeiling->AddComponent(new RigidBodyComponent());
		pRigidBody->SetConstraint(RigidBodyConstraint::All, false);
		pRigidBody->AddCollider(PxBoxGeometry{ dimensions.x * 0.5f,dimensions.y * 0.5f,dimensions.z * 0.5f }, *pDefaultMat);
		pRigidBody->SetKinematic(true);

		const auto pFloor = new GameObject();
		AddChild(pFloor);
		pRigidBody = pFloor->AddComponent(new RigidBodyComponent());
		pRigidBody->SetConstraint(RigidBodyConstraint::All, false);
		pRigidBody->AddCollider(PxBoxGeometry{ dimensions.x * 0.5f,dimensions.y * 0.5f,dimensions.z * 0.5f }, *pDefaultMat);
		pRigidBody->SetKinematic(true);

		pCeiling->GetTransform()->Translate(XMVECTOR{ 0,0,displacement });
		pFloor->GetTransform()->Translate(XMVECTOR{ 0,0,-displacement });
	}

	// Paddles
	{
		XMFLOAT3 paddleDimensions{ 3,3,6 };
		m_pPaddleLeft = new CubePrefab(paddleDimensions, XMFLOAT4{ Colors::White });
		m_pPaddleRight = new CubePrefab(paddleDimensions, XMFLOAT4{ Colors::White });

		m_pPaddleLeft->GetTransform()->Translate(XMVECTOR{ -20,0,0 });
		m_pPaddleRight->GetTransform()->Translate(XMVECTOR{ 20,0,0 });

		AddChild(m_pPaddleLeft);
		AddChild(m_pPaddleRight);

		auto pRigidBody = m_pPaddleLeft->AddComponent(new RigidBodyComponent());
		pRigidBody->SetConstraint(RigidBodyConstraint::All, false);
		pRigidBody->AddCollider(PxBoxGeometry{ paddleDimensions.x * 0.5f,paddleDimensions.y * 0.5f,paddleDimensions.z * 0.5f }, *pDefaultMat);
		pRigidBody->SetKinematic(true);

		pRigidBody = m_pPaddleRight->AddComponent(new RigidBodyComponent());
		pRigidBody->SetConstraint(RigidBodyConstraint::All, false);
		pRigidBody->AddCollider(PxBoxGeometry{ paddleDimensions.x * 0.5f,paddleDimensions.y * 0.5f,paddleDimensions.z * 0.5f }, *pDefaultMat);
		pRigidBody->SetKinematic(true);
	}

	// Ball
	{
		const constexpr float ballRadius{ 1.f };
		const constexpr int ballSteps{ 10 };
		m_pBall = new SpherePrefab(ballRadius, ballSteps, XMFLOAT4{ Colors::Red });
		AddChild(m_pBall);

		auto pRigidBody = m_pBall->AddComponent(new RigidBodyComponent());
		pRigidBody->SetConstraint(RigidBodyConstraint::TransY, false);
		pRigidBody->SetConstraint(RigidBodyConstraint::AllRot, false);
		pRigidBody->AddCollider(PxSphereGeometry{ ballRadius }, *pDefaultMat);
	}

	const auto pFixedCamera = new FixedCamera();
	pFixedCamera->GetTransform()->Translate(0, 40.f, 0);
	pFixedCamera->GetTransform()->Rotate(90, 0, 0);
	AddChild(pFixedCamera);
	SetActiveCamera(pFixedCamera->GetComponent<CameraComponent>());
}

void PongScene::Update()
{
	const auto leftPaddleTransform = m_pPaddleLeft->GetTransform();
	const auto rightPaddleTransform = m_pPaddleRight->GetTransform();
	auto leftPos = leftPaddleTransform->GetPosition();
	auto rightPos = rightPaddleTransform->GetPosition();

	const auto pInput = m_SceneContext.pInput;
	if (pInput->IsKeyboardKey(InputState::down, 'W'))
		leftPos.z += m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();

	if (pInput->IsKeyboardKey(InputState::down, 'S'))
		leftPos.z -= m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();

	if (pInput->IsKeyboardKey(InputState::pressed, ' '))
		m_pBall->GetComponent<RigidBodyComponent>()->AddForce({ m_BallSpeed,0,m_BallSpeed / 2 }, PxForceMode::eIMPULSE);

	if (pInput->IsKeyboardKey(InputState::down,VK_UP))
		rightPos.z += m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();
	if (pInput->IsKeyboardKey(InputState::down, VK_DOWN))
		rightPos.z -= m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();

	auto ballPos = m_pBall->GetTransform()->GetPosition();
	if (InputManager::IsKeyboardKey(InputState::pressed, 'R') || std::abs(ballPos.x) > 25 || std::abs(ballPos.z) > 25)
	{
		Reset();
	}
	


	leftPaddleTransform->Translate(leftPos);
	rightPaddleTransform->Translate(rightPos);
}

void PongScene::Draw()
{
	//Optional
}

void PongScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}

void PongScene::Reset() const
{
	m_pPaddleLeft->GetTransform()->Translate(XMVECTOR{ -20,0,0 });
	m_pPaddleRight->GetTransform()->Translate(XMVECTOR{ 20,0,0 });
	m_pBall->GetTransform()->Translate(XMVECTOR{ 0,0,0 });
	m_pBall->GetComponent<RigidBodyComponent>()->AddForce(XMFLOAT3{ 10,0,10 }, PxForceMode::eIMPULSE);
}