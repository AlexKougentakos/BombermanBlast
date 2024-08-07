#include "stdafx.h"
#include "BombPrefab.h"

#include "BombermanCharacter.h"
#include "Materials/DiffuseMaterial.h"
#include "Components/Grid.h"
#include "Explosion.h"
#include "Components/GameObjectManager.h"

BombPrefab::BombPrefab(int blastRadius, GridComponent* pGridComponent, BombermanCharacter* playerWhoPlacedBomb)
	:m_BlastRadius(blastRadius),
	m_pGrid(pGridComponent),
	m_pPlayerWhoPlacedBomb(playerWhoPlacedBomb)
{
	SetTag(L"Bomb");
}

void BombPrefab::Initialize(const SceneContext& /*gameContext*/)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();

	const auto pBombBase = new GameObject();
	pDiffuseMat->SetDiffuseTexture(L"Textures/Bomb/BombTexture.png");
	m_pBombModelComponent = pBombBase->AddComponent(new ModelComponent(L"Meshes/Bomb/Bomb_Base.ovm"));
	m_pBombModelComponent->SetMaterial(pDiffuseMat);
	m_pBombModelComponent->GetTransform()->Scale(0.33f);
	AddChild(pBombBase);
	
	m_pBombFuse = new GameObject();
	m_pFuseModelComponent = m_pBombFuse->AddComponent(new ModelComponent(L"Meshes/Bomb/Bomb_Top.ovm"));
	m_pFuseModelComponent->SetMaterial(pDiffuseMat);
	m_pFuseModelComponent->GetTransform()->Scale(0.33f);
	AddChild(m_pBombFuse);

	//Rotate the bomb to face the camera
	pBombBase->GetTransform()->Rotate(XMLoadFloat4(&GetScene()->GetActiveCamera()->GetTransform()->GetRotation()));

	//Set up collision
	m_pRigidBody = new RigidBodyComponent(true);
	AddComponent(m_pRigidBody);
	m_pRigidBody->SetCollisionGroup(CollisionGroup::Bomb_Inside); //This group gets ignored by the player
	const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.2f, 0.2f, 0.2f);
	const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Bomb/Collider.ovpc");
	m_pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ .03f,.03f,.03f })), *physicsMat);
}

void BombPrefab::InitializeParticles()
{	
	// Flash Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,0.f,0.f };
	settings.minSize = 7.f;
	settings.maxSize = 7.f;
	settings.minEnergy = 0.f;
	settings.maxEnergy = 0.1f;
	settings.minScale = 1.f;
	settings.maxScale = 2.f;
	settings.minEmitterRadius = 0.f;
	settings.maxEmitterRadius = 0.f;
	settings.color = { 1.f,1.f,1.f,.3f };

	const XMFLOAT3 bombPosition = GetTransform()->GetPosition();
	const XMFLOAT3 placementPosition = { bombPosition.x, bombPosition.y + 10.f, bombPosition.z };
	GameObject* pFuseOrangeParticleContainer = new GameObject();
	AddChild(pFuseOrangeParticleContainer);
	pFuseOrangeParticleContainer->GetTransform()->Translate(placementPosition);
	pFuseOrangeParticleContainer->AddComponent(new ParticleEmitterComponent(L"Textures/Bomb/FuseOrangeParticle.png", settings, 7));

	// Flame Particle System #1
	settings.velocity = { 0.f,0.f,0.f };
	settings.minSize = 2.5f;
	settings.maxSize = 2.5f;
	settings.minEnergy = 0.f;
	settings.maxEnergy = 0.1f;
	settings.minScale = 1.f;
	settings.maxScale = 2.f;
	settings.minEmitterRadius = 0.f;
	settings.maxEmitterRadius = 0.f;
	settings.color = { 1.f,1.f,1.f,.7f };

	GameObject* pFuseYellowParticle1Container = new GameObject();
	AddChild(pFuseYellowParticle1Container);
	pFuseYellowParticle1Container->GetTransform()->Translate(placementPosition);
	pFuseYellowParticle1Container->AddComponent(new ParticleEmitterComponent(L"Textures/Bomb/FuseYellowParticle_1.png", settings, 5));

	// Flame Particle System #2
	settings.velocity = { 0.f,0.f,0.f };
	settings.minSize = 2.5f;
	settings.maxSize = 2.5f;
	settings.minEnergy = 0.f;
	settings.maxEnergy = 0.5f;
	settings.minScale = 2.f;
	settings.maxScale = 1.f;
	settings.minEmitterRadius = 0.f;
	settings.maxEmitterRadius = 0.f;
	settings.color = { 1.f,1.f,1.f,.35f };

	GameObject* pFuseYellowParticle2Container = new GameObject();
	AddChild(pFuseYellowParticle2Container);
	pFuseYellowParticle2Container->GetTransform()->Translate(placementPosition);
	pFuseYellowParticle2Container->AddComponent(
		new ParticleEmitterComponent(L"Textures/Bomb/FuseYellowParticle_2.png", settings, 1));
}

void BombPrefab::Update(const SceneContext& sceneContext)
{
	const float elapsedSec = sceneContext.pGameTime->GetElapsed();
	m_FuseElapsedTime += elapsedSec;

	const XMFLOAT3 playerPosition = m_pPlayerWhoPlacedBomb->GetTransform()->GetPosition();
	const XMFLOAT3 bombPosition = GetTransform()->GetPosition();
	const double distance = sqrt(pow(playerPosition.x - bombPosition.x, 2) + pow(playerPosition.z - bombPosition.z, 2));
	if (distance > 6.f && m_ExitedBomb == false)
	{
		m_ExitedBomb = true;
		m_pRigidBody->SetCollisionGroup(CollisionGroup::Bomb_Outside); //This group is NOT ignored by the player
	}
		
	
	//Initialize the particles here because the particle system cannot move during run time
	//and the bomb is placed a frame after the initialization
	if (!m_InitializedParticles && !GetTransform()->IsDirty())
	{
		InitializeParticles();
		m_InitializedParticles = true;
	}
	
	AnimateBombScaling(elapsedSec);
	
	if (m_FuseElapsedTime >= m_ExplosionTime)
	{
		Explode(m_BlastRadius);
	}
}

void BombPrefab::Explode(int explosionDistance)
{
	const auto bombPosition = GetTransform()->GetPosition();
	auto& cell = m_pGrid->GetCell(bombPosition);

	std::vector<GridCell> affectedCells{};
	affectedCells.emplace_back(cell);

	ExplodeRecursive(cell, explosionDistance, affectedCells);

	for (auto& affectedCell : affectedCells)
	{
		if (affectedCell.isValid)
		{
			m_pGrid->Explode(affectedCell);
			m_pGrid->PlaceObject(m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->CreateGameObject<Explosion>(m_pGrid), affectedCell);
		}
	}

	//Recharge player bomb
	m_pPlayerWhoPlacedBomb->GiveBackBomb();

	SoundManager::Get()->Play(L"Explosion.wav", 0.5f);
	cell.Remove(this);
	m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(this);
}

void BombPrefab::AnimateBombScaling(const float elapsedSec)
{
	m_ScaleElapsedTime += elapsedSec * m_ScaleSpeed;

	//Use a sin wave to animate the bomb scale
	const float scaleFactor = sinf(m_ScaleElapsedTime * XM_PI) * 0.5f + 0.5f;
	const float newScale = (1 - m_Amplitude) + (scaleFactor * m_Amplitude);
	
	const XMFLOAT3 mScale = GetTransform()->GetScale();
	GetTransform()->Scale(mScale.x, newScale, mScale.z);
}

void BombPrefab::ExplodeRecursive(const GridCell& cell, int explosionDistance, std::vector<GridCell>& affectedCellsOut) const
{
	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findLeft;
	findLeft = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0 || cell.Contains(L"Rock"))
			return;
		GridCell leftCell = m_pGrid->GetCellToTheLeft(cell);
		if (leftCell.isValid && !leftCell.Contains(L"UnbreakableRock"))
		{
			affectedCellsOut.emplace_back(leftCell);
			findLeft(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findLeft(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findUp;
	findUp = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0 || cell.Contains(L"Rock"))
			return;
		GridCell leftCell = m_pGrid->GetCellOnTop(cell);
		if (leftCell.isValid && !leftCell.Contains(L"UnbreakableRock"))
		{
			affectedCellsOut.emplace_back(leftCell);
			findUp(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findUp(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findDown;
	findDown = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0 || cell.Contains(L"Rock"))
			return;
		GridCell leftCell = m_pGrid->GetCellUnder(cell);
		if (leftCell.isValid && !leftCell.Contains(L"UnbreakableRock"))
		{
			affectedCellsOut.emplace_back(leftCell);
			findDown(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findDown(cell, explosionDistance, affectedCellsOut);

	std::function<void(const GridCell&, int, std::vector<GridCell>&)> findRight;
	findRight = [&](const GridCell& cell, int distance, std::vector<GridCell>& affectedCellsOut)
	{
		if (distance <= 0 || cell.Contains(L"Rock"))
			return;
		GridCell leftCell = m_pGrid->GetCellToTheRight(cell);
		if (leftCell.isValid && !leftCell.Contains(L"UnbreakableRock"))
		{
			affectedCellsOut.emplace_back(leftCell);
			findRight(leftCell, distance - 1, affectedCellsOut);
		}
	};

	findRight(cell, explosionDistance, affectedCellsOut);
}

bool BombPrefab::CanContinueExplosion(const GridCell& cell) const
{
	return cell.isValid && !cell.Contains(L"UnbreakableRock") && !cell.Contains(L"SkullBoxLanded");
}
