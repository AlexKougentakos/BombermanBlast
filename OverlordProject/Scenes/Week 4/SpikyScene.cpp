#include "stdafx.h"
#include "SpikyScene.h"

#include "Materials/SpikyMaterial.h"

void SpikyScene::Initialize()
{
	m_pSphere = new GameObject();

	const auto pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	m_pSphere->GetTransform()->Scale(15.f, 15.f, 15.f);
	//Add a model component to the sphere
	const auto pModel =  m_pSphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	AddChild(m_pSphere);

	pModel->SetMaterial(pSpikyMaterial->GetMaterialId());
}	

void SpikyScene::Update()
{

}
