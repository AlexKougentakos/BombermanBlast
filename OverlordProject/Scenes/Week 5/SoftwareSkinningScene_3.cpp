#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	const auto pRoot = new GameObject();
	AddChild(pRoot);

	constexpr float boneLength{ 15.f };

	m_pBone1 = new BoneObject(pColorMat, boneLength);
	pRoot->AddChild(m_pBone1);
	m_pBone2 = new BoneObject(pColorMat, boneLength);
	m_pBone1->AddBone(m_pBone2);

	const auto pBoxDrawer = new GameObject();
	AddChild(pBoxDrawer);

	m_pMeshDrawer = pBoxDrawer->AddComponent(new MeshDrawComponent(24, true));
	m_pBone1->CalculateBindPose();

	InitializeVertices(boneLength);
}

void SoftwareSkinningScene_3::Update()
{
	if (m_AutoRotate)
	{
		constexpr float rotationSpeed{ 45.0f };
		constexpr float maxAngle{ 45.0f };

		m_BoneRotation += rotationSpeed * m_RotationSign * m_SceneContext.pGameTime->GetElapsed();

		if (std::abs(m_BoneRotation) > maxAngle)
		{
			m_BoneRotation -= m_RotationSign * (std::abs(m_BoneRotation) - maxAngle);
			m_RotationSign = -m_RotationSign;
		}

		m_Bone1Rot = XMFLOAT3{ 0.0f, 0.0f, m_BoneRotation };
		m_Bone2Rot = XMFLOAT3{ 0.0f, 0.0f, -m_BoneRotation * 2.0f };
	}

	UpdateBones();

	UpdateMeshDrawer();
}

void SoftwareSkinningScene_3::UpdateMeshDrawer() const
{
	m_pMeshDrawer->RemoveTriangles();

	// Add quads to draw
	for (int currentQuad{}; currentQuad < m_SkinnedVertices.size(); currentQuad += 4)
	{
		const QuadPosNormCol quad
		{
			m_SkinnedVertices[currentQuad].transformedVertex,
			m_SkinnedVertices[currentQuad + 1].transformedVertex,
			m_SkinnedVertices[currentQuad + 2].transformedVertex,
			m_SkinnedVertices[currentQuad + 3].transformedVertex
		};
		m_pMeshDrawer->AddQuad(quad);
	}

	// Update the buffer
	m_pMeshDrawer->UpdateBuffer();
}


void SoftwareSkinningScene_3::UpdateBones()
{
	m_pBone1->GetTransform()->Rotate(m_Bone1Rot);
	m_pBone2->GetTransform()->Rotate(m_Bone2Rot);

	const XMMATRIX bone1Transform{ XMLoadFloat4x4(&m_pBone1->GetBindPose()) * XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld()) };
	const XMMATRIX bone2Transform{ XMLoadFloat4x4(&m_pBone2->GetBindPose()) * XMLoadFloat4x4(&m_pBone2->GetTransform()->GetWorld()) };

	// Iterate over each vertex and transform its position with the corresponding bone
	for (int currentVertex{}; currentVertex < m_SkinnedVertices.size(); ++currentVertex)
	{
		VertexSoftwareSkinned& skinnedVertex{ m_SkinnedVertices[currentVertex] };

		if (skinnedVertex.blendWeight > 0.0f && skinnedVertex.blendWeight < 1.0f)
		{
			const XMVECTOR transformedVertexBone1{ XMVector3TransformCoord(XMLoadFloat3(&skinnedVertex.originalVertex.Position), bone1Transform) };
			const XMVECTOR transformedVertexBone2{ XMVector3TransformCoord(XMLoadFloat3(&skinnedVertex.originalVertex.Position), bone2Transform) };
			XMStoreFloat3(&skinnedVertex.transformedVertex.Position, XMVectorLerp(transformedVertexBone2, transformedVertexBone1, skinnedVertex.blendWeight));
		}
		else
		{
			const XMMATRIX& curBoneTransform{ currentVertex / 24 == 0 ? bone1Transform : bone2Transform };

			const XMVECTOR transformedVertex{ XMVector3TransformCoord(XMLoadFloat3(&skinnedVertex.originalVertex.Position), curBoneTransform) };
			XMStoreFloat3(&skinnedVertex.transformedVertex.Position, transformedVertex);
		}
	}
}

void SoftwareSkinningScene_3::OnGUI()
{
	// Disable the rotation if AutoRotate is enabled
	ImGui::BeginDisabled(m_AutoRotate);

	// Input for rotations
	ImGui::DragFloat3("Bone 1 - ROT", reinterpret_cast<float*>(&m_Bone1Rot));
	ImGui::DragFloat3("Bone 2 - ROT", reinterpret_cast<float*>(&m_Bone2Rot));

	ImGui::EndDisabled();

	// Input to enable/disable auto rotation
	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
#pragma endregion
}