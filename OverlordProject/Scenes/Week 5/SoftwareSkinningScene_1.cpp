#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	// Create a material for the bones
	ColorMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };
	pMaterial->SetColor(XMFLOAT4{ 0.5f, 0.5f, 0.5f, 1.0f });

	// Create a root object for the scene
	GameObject* pRoot{ new GameObject{} };
	AddChild(pRoot);

	constexpr float boneSize{ 15.f };

	// Initialize the first bone object
	m_pBone1 = new BoneObject{ pMaterial, boneSize };
	pRoot->AddChild(m_pBone1);

	// Initialize the second bone object
	m_pBone2 = new BoneObject{ pMaterial, boneSize };
	m_pBone1->AddBone(m_pBone2);
}

void SoftwareSkinningScene_1::Update()
{
	if (m_AutoRotate)
	{
		constexpr float rotationSpeed{ 45.0f };
		constexpr float maxAngle{ 45.0f };
		m_BoneRotation += rotationSpeed * m_RotationDirection * m_SceneContext.pGameTime->GetElapsed();

		if (std::abs(m_BoneRotation) > maxAngle)
		{
			m_BoneRotation -= m_RotationDirection * (std::abs(m_BoneRotation) - maxAngle);
			m_RotationDirection = -m_RotationDirection;
		}

		m_Bone1Rotation = XMFLOAT3{ 0.0f, 0.0f, m_BoneRotation };
		m_Bone2Rotation = XMFLOAT3{ 0.0f, 0.0f, -m_BoneRotation * 2.0f };
	}

	m_pBone1->GetTransform()->Rotate(m_Bone1Rotation);
	m_pBone2->GetTransform()->Rotate(m_Bone2Rotation);

}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::BeginDisabled(m_AutoRotate);
	ImGui::DragFloat3("Bone 1 - ROT", reinterpret_cast<float*>(&m_Bone1Rotation));
	ImGui::DragFloat3("Bone 2 - ROT", reinterpret_cast<float*>(&m_Bone2Rotation));
	ImGui::EndDisabled();

	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
}