#include "stdafx.h"
#include "BombPrefab.h"

#include "CubePrefab.h"
#include "Materials/DiffuseMaterial.h"
#include "Components/Grid.h"

BombPrefab::BombPrefab(int blastRadius, GridComponent* pGridComponent)
	:m_BlastRadius(blastRadius),
	m_pGrid(pGridComponent)
{
	SetTag(L"Bomb");
}

void BombPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pDiffuseMat->SetDiffuseTexture(L"Textures/Bomb.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/Bomb.ovm"));
	[[maybe_unused]]const auto pRigidBody = AddComponent(new RigidBodyComponent(true));

	m_pModelComponent->SetMaterial(pDiffuseMat);

	

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

void BombPrefab::Explode(int explosionDistance) const
{
	const auto bombPosition = GetTransform()->GetPosition();
	auto cell = m_pGrid->GetCell(bombPosition);

	std::vector<GridCell> affectedCells{};

	ExplodeRecursive(cell, explosionDistance, affectedCells);

	for (auto& affectedCell : affectedCells)
	{
		//todo: add particle effects
		//Bombs don't affect other bombs
		if (m_pGrid->GetObjectOnCell(affectedCell) == 'B')
			continue;
		if (affectedCell.isValid)
			m_pGrid->RemoveObject(affectedCell);
	}

	m_pGrid->RemoveObject(cell);

}

void BombPrefab::ExplodeRecursive(const GridCell& cell, int explosionDistance, std::vector<GridCell>& affectedCellsOut) const
{
	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findLeft;
	findLeft = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0)
			return;
		GridCell leftCell = m_pGrid->GetCellToTheLeft(cell);
		if (leftCell.isValid)
		{
			affectedCellsOut.emplace_back(leftCell);
			findLeft(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findLeft(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findUp;
	findUp = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0)
			return;
		GridCell leftCell = m_pGrid->GetCellOnTop(cell);
		if (leftCell.isValid)
		{
			affectedCellsOut.emplace_back(leftCell);
			findUp(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findUp(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findDown;
	findDown = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0)
			return;
		GridCell leftCell = m_pGrid->GetCellUnder(cell);
		if (leftCell.isValid)
		{
			affectedCellsOut.emplace_back(leftCell);
			findDown(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findDown(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findRight;
	findRight = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0)
			return;
		GridCell leftCell = m_pGrid->GetCellToTheRight(cell);
		if (leftCell.isValid)
		{
			affectedCellsOut.emplace_back(leftCell);
			findRight(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findRight(cell, explosionDistance, affectedCellsOut);
}
