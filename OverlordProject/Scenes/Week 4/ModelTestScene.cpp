#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

void ModelTestScene::Initialize()
{
	auto& physX{ PxGetPhysics() };
	auto pDefaultMat{ physX.createMaterial(0.5f, 0.5f, 0.5f) };

	//Create the game object
	m_pObject = new GameObject;
	AddChild(m_pObject);

	//Create a new colour material and add it to the material manager
	//const auto pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	//Create a new diffuse material
	const auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();

	//Add a model component to the game object
	const auto pModel = m_pObject->AddComponent(new ModelComponent(L"Meshes/Chair.ovm"));

	//Attach Ridigbody to the object
	const auto pRigidBody = m_pObject->AddComponent(new RigidBodyComponent());
	m_pObject->GetTransform()->Translate(0.f, 10.f, 0.f);

	//Create the convex mesh
	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	const auto convexGeometry{ PxConvexMeshGeometry{pConvexMesh} };

	//Add the collider to the object
	pRigidBody->AddCollider(convexGeometry, *pDefaultMat);

	pDiffuseMat->SetDiffuseTexture(L"Textures/Chair_Dark.dds");
	pModel->SetMaterial(pDiffuseMat->GetMaterialId());

	//Add a colliding ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMat);
}

void ModelTestScene::Update()
{
	
}

void ModelTestScene::OnGUI()
{
	
}