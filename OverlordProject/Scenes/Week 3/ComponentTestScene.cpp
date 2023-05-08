#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"

void ComponentTestScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	auto& physX = PxGetPhysics();
	const auto pBouncyMaterial = physX.createMaterial(0.5f, 0.5f, 1.f);

	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Sphere
	const auto pSphereRed = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
	AddChild(pSphereRed);

	pSphereRed->GetTransform()->Translate(0, 30.f, 0);
	const auto pRigidBody = pSphereRed->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1);
	//pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ);

	//Green
	const auto pSphereGreen = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Green });
	AddChild(pSphereGreen);

	pSphereGreen->GetTransform()->Translate(0, 20.f, 0);
	const auto pRigidBody2 = pSphereGreen->AddComponent(new RigidBodyComponent(false));
	pRigidBody2->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody2->SetCollisionGroup(CollisionGroup::Group1);
}

void ComponentTestScene::Update()
{

}

void ComponentTestScene::Draw()
{
	//Optional
}

void ComponentTestScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
