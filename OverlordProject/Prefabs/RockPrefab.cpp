#include "stdafx.h"
#include "RockPrefab.h"
#include "Components/Grid.h"

#include "Materials/Shadow/DiffuseMaterial_ShadowFixed.h"

RockPrefab::RockPrefab(RockType type, float scale)
{
	m_Type = type;
	m_Scale = scale;
}

void RockPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_ShadowFixed>(XMFLOAT3{0, -1, -1});
	pDiffuseMat->SetDiffuseTexture(L"Textures/brick.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/Brick.ovm"));
	const auto pRigidBody = AddComponent(new RigidBodyComponent(true));

	pRigidBody->SetCollisionGroup(CollisionGroup::Brick);

	m_pModelComponent->SetMaterial(pDiffuseMat);

	const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);

	const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Brick.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ m_Scale,m_Scale,m_Scale })), *physicsMat);

	//Scale if necessary
	m_pModelComponent->GetTransform()->Scale(m_Scale);

	SetTag(L"Rock");
}

void RockPrefab::Explode() const
{
	m_pCurrentCell->Remove(this);
}