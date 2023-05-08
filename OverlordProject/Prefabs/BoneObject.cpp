#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	:GameObject(),
	m_Length(length),
	m_pMaterial(pMaterial)
{
	
}

void BoneObject::Initialize(const SceneContext&)
{
	const auto pEmpty = new GameObject{};
	AddChild(pEmpty);

	const auto pModel = pEmpty->AddComponent(new ModelComponent{ L"Meshes/Bone.ovm" });
	pModel->SetMaterial(m_pMaterial);

	pEmpty->GetTransform()->Rotate(0	, -90, 0);
	pEmpty->GetTransform()->Scale(m_Length);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Length, 0, 0);
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	// Calculate the inverse world matrix
	const XMMATRIX inverseWorldMatrix{ XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld())) };

	// Store the matrix
	XMStoreFloat4x4(&m_BindPose, inverseWorldMatrix);

	// Calculate the bind pose of each child
	for (BoneObject* pChild : GetChildren<BoneObject>())
	{
		pChild->CalculateBindPose();
	}
}


