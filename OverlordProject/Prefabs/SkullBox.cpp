#include "stdafx.h"
#include "SkullBox.h"

#include "BombermanCharacter.h"
#include "Components/Grid.h"
#include "Materials/Shadow/DiffuseMaterial_ShadowFixed.h"

SkullBox::SkullBox(const float scale, GridComponent* pGridComponent)
	: m_Scale(scale),
	m_pGridComponent(pGridComponent)
{
	
}

void SkullBox::Initialize(const SceneContext&)
{
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_ShadowFixed>(XMFLOAT3{0, -1, -1});
	pDiffuseMat->SetDiffuseTexture(L"Textures/SkullBox.png");

	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/SkullBox.ovm"));

	const auto pRigidBody = AddComponent(new RigidBodyComponent());
	//pRigidBody->SetConstraint(RigidBodyConstraint::AllRot | RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);
	pRigidBody->SetKinematic(true);
	
	pRigidBody->SetCollisionGroup(CollisionGroup::Brick);

	m_pModelComponent->SetMaterial(pDiffuseMat);

	const auto physicsMat = PhysXManager::Get()->GetPhysics()->createMaterial(1.f, 1.f, 0.01f);
	const auto pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/SkullBox.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry(pPxConvexMesh, PxMeshScale({ m_Scale,m_Scale,m_Scale })), *physicsMat);
	
	
	//Scale if necessary
	m_pModelComponent->GetTransform()->Scale(m_Scale);
	SetTag(L"SkullBoxFalling");
}


void SkullBox::Update(const SceneContext& sceneContext)
{
	if (!m_OnGround)
	{
		const XMFLOAT3 currentPos = GetTransform()->GetPosition();
		GetTransform()->Translate(currentPos.x, currentPos.y - m_FallingSpeed * sceneContext.pGameTime->GetElapsed(), currentPos.z);
		if (currentPos.y < 8.f && !m_OnGround)
		{
			m_pGridComponent->Explode(m_pGridComponent->GetCell(*this), false);
		}
		if (currentPos.y < 0.f)
		{
			m_OnGround = true;
			GetTransform()->Translate(currentPos.x, 0.f, currentPos.z);
			SetTag(L"SkullBoxLanded");
		}
	}
}

void SkullBox::PlacedInGrid()
{
	const XMFLOAT3 currentPos = GetTransform()->GetPosition();
	GetTransform()->Translate(currentPos.x, 86.f, currentPos.z);
	
}