#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	const auto pShadowMapRenderer{ ShadowMapRenderer::Get() };
	const XMFLOAT4X4 modelWorld{ pModel->GetTransform()->GetWorld() };
	const XMFLOAT4X4 lightViewProjection{ pShadowMapRenderer->GetLightVP() };
	XMFLOAT4X4 lightWorldViewProjection{};
	XMStoreFloat4x4(&lightWorldViewProjection, XMLoadFloat4x4(&modelWorld) * XMLoadFloat4x4(&lightViewProjection));
	SetVariable_Matrix(L"gWorldViewProj_Light", lightWorldViewProjection);

	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}
