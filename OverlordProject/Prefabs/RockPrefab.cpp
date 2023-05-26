#include "stdafx.h"
#include "RockPrefab.h"

#include "Materials/DiffuseMaterial.h"

RockPrefab::RockPrefab(RockType type, float scale)
{
	m_Type = type;
	m_Scale = scale;
}

void RockPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pDiffuseMat->SetDiffuseTexture(L"Textures/brick.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/Brick.ovm"));
	const auto pRigidBody = AddComponent(new RigidBodyComponent(true));

	m_pModelComponent->SetMaterial(pDiffuseMat);

	const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);

	const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Brick.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ m_Scale,m_Scale,m_Scale })), *physicsMat);

	//Scale if necessary
	m_pModelComponent->GetTransform()->Scale(m_Scale);

	SetTag(L"Rock");

}

void RockPrefab::Update(const SceneContext& /*gameContext*/)
{
	
}
