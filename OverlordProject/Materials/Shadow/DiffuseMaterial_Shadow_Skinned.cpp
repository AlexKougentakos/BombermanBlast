#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables([[maybe_unused]]const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	const auto animator = pModel->GetAnimator();
	ASSERT_NULL_(animator);

	const auto boneTransforms = animator->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", &boneTransforms[0]._11, static_cast<UINT>(boneTransforms.size()));
}
