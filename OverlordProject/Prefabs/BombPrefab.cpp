#include "stdafx.h"
#include "BombPrefab.h"

#include "Materials/DiffuseMaterial.h"
#include "Components/Grid.h"

BombPrefab::BombPrefab(int blastRadius, GridComponent* pGridComponent)
	:m_BlastRadius(blastRadius),
	m_pGrid(pGridComponent)
{}

void BombPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pDiffuseMat->SetDiffuseTexture(L"Textures/Bomb.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/Bomb.ovm"));
	[[maybe_unused]]const auto pRigidBody = AddComponent(new RigidBodyComponent(true));

	m_pModelComponent->SetMaterial(pDiffuseMat);

	SetTag(L"Bomb");

	//todo: add collider after
	//const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);
	//const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Bomb.ovpc");
	//pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ 1.f,1.f,1.f })), *physicsMat);
}

void BombPrefab::Update(const SceneContext& sceneContext)
{
	m_FuseElapsedTime += sceneContext.pGameTime->GetElapsed();

	if (m_FuseElapsedTime >= m_ExplosionTime)
	{
		Explode(m_BlastRadius);
	}
}

void BombPrefab::Explode(int /*explosionDistance*/) const
{
	const auto bombPosition = GetTransform()->GetPosition();
	const auto cell = m_pGrid->GetCell(bombPosition);

	std::vector<GridCell> affectedCells{};

}