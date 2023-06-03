#include "stdafx.h"
#include "DiffuseMaterial_ShadowFixed.h"

DiffuseMaterial_ShadowFixed::DiffuseMaterial_ShadowFixed(const XMFLOAT3& lightDir) :
	Material(L"Effects/Shadow/PosNormTex3D_FixedShadow.fx"),
	m_LightDirection(lightDir)
{
	
}

void DiffuseMaterial_ShadowFixed::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_ShadowFixed::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", m_LightDirection);
}

//todo: Fix this or remove it
void DiffuseMaterial_ShadowFixed::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{
	const auto pShadowMapRenderer{ ShadowMapRenderer::Get() };
	const XMFLOAT4X4 modelWorld{ pModel->GetTransform()->GetWorld() };
	const XMFLOAT4X4 lightViewProjection{ pShadowMapRenderer->GetLightVP() };
	XMFLOAT4X4 lightWorldViewProjection{};
	XMStoreFloat4x4(&lightWorldViewProjection, XMLoadFloat4x4(&modelWorld) * XMLoadFloat4x4(&lightViewProjection));
	//SetVariable_Matrix(L"gWorldViewProj_Light", lightWorldViewProjection);

	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
}