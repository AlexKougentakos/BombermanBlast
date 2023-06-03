#include "stdafx.h"
#include "PowerUp.h"

#include "Components/Grid.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/BombermanCharacter.h"

BasePowerUp::BasePowerUp(GridComponent* pGridComponent):
	m_pGrid(pGridComponent)
{
	SetTag(L"PowerUp");
}

void BasePowerUp::Initialize(const SceneContext& /*sceneContext*/)
{
	//All of the powerUps will have the same model. Material needs to be set in the derived class.
	m_pModelComponent = AddComponent(new ModelComponent(L"Meshes/PowerUp.ovm"));

	m_pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	m_pMaterial->SetDiffuseTexture(m_TexturePath);

	GetTransform()->Scale(m_pGrid->GetScaleFactor());
	m_pModelComponent->SetMaterial(m_pMaterial);

	GetTransform()->Rotate(45, 0, 0);
	
	auto& physx = PxGetPhysics();
	const auto physicsMat = physx.createMaterial(0.2f, 0.2f, 0.f);

	const float scale = m_pGrid->GetScaleFactor() / 2;
	m_pRigidBody = AddComponent(new RigidBodyComponent(true));
	m_pRigidBody->AddCollider(PxBoxGeometry(m_pModelComponent->CalculateWidth() * scale,m_pModelComponent->CalculateHeight() * scale * 2, m_pModelComponent->CalculateDepth() * scale), *physicsMat, true);

	SetOnTriggerCallBack(std::bind(&BasePowerUp::OnCollision, this, std::placeholders::_1, 
		std::placeholders::_2, std::placeholders::_3));
}


void BasePowerUp::SetTexture(const std::wstring& texturePath)
{
	m_TexturePath = texturePath;
}