#include "stdafx.h"
#include "BombPrefab.h"

#include "Materials/DiffuseMaterial.h"

BombPrefab::BombPrefab()
{

}

void BombPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pDiffuseMat->SetDiffuseTexture(L"Textures/Bomb.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/Bomb.ovm"));
	const auto pRigidBody = AddComponent(new RigidBodyComponent(true));

	m_pModelComponent->SetMaterial(pDiffuseMat);

	const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);

	const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Bomb.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ 1.f,1.f,1.f })), *physicsMat);

}

void BombPrefab::Update(const SceneContext& /*gameContext*/)
{
	
}