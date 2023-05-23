#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRenderTarget)
}

void ShadowMapRenderer::Initialize()
{
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
	//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
	//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height)
	RENDERTARGET_DESC renderTargetDesc {};
	renderTargetDesc.enableColorBuffer = false;
	renderTargetDesc.enableDepthSRV = true;
	renderTargetDesc.width = 1280;
	renderTargetDesc.height = 720;
	m_pShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	m_pShadowRenderTarget->Create(renderTargetDesc);

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)
	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>();
	m_GeneratorTechniqueContexts[static_cast<int>(ShadowGeneratorType::Static)] = m_pShadowMapGenerator->GetTechniqueContext(static_cast<int>(ShadowGeneratorType::Static));
	m_GeneratorTechniqueContexts[static_cast<int>(ShadowGeneratorType::Skinned)] = m_pShadowMapGenerator->GetTechniqueContext(static_cast<int>(ShadowGeneratorType::Skinned));
}

void ShadowMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
		ShadowGeneratorType type {
		pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static
	};

	const MaterialTechniqueContext& technique{ m_GeneratorTechniqueContexts[static_cast<int>(type)] };

	pMeshFilter->BuildVertexBuffer(sceneContext, technique.inputLayoutID, technique.inputLayoutSize, technique.pInputLayoutDescriptions);
}

void ShadowMapRenderer::Begin(const SceneContext& sceneContext)
{
	//This function is called once right before we start the Shadow Pass (= generating the ShadowMap)
	//This function is responsible for setting the pipeline into the correct state, meaning
	//	- Making sure the ShadowMap is unbound from the pipeline as a ShaderResource (SRV), so we can bind it as a RenderTarget (RTV)
	//	- Calculating the Light ViewProjection, and updating the relevant Shader variables
	//	- Binding the ShadowMap RenderTarget as Main Game RenderTarget (= Everything we render is rendered to this rendertarget)
	//	- Clear the current (which should be the ShadowMap RT) rendertarget

	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	const XMMATRIX lightProjMatrix{ XMMatrixOrthographicLH(sceneContext.aspectRatio * 100.0f, 100.0f, 0.1f, 500.0f) };
	const Light& dirLight{ sceneContext.pLights->GetDirectionalLight() };
	const XMVECTOR lightPos{ XMLoadFloat4(&dirLight.position) };
	const XMVECTOR lightDir{ XMLoadFloat4(&dirLight.direction) };
	const XMMATRIX lightViewMatrix{ XMMatrixLookAtLH(lightPos, lightPos + lightDir, XMVECTOR{0.0f,1.0f,0.0f}) };
	const XMMATRIX lightVP{ lightViewMatrix * lightProjMatrix };
	XMStoreFloat4x4(&m_LightVP, lightVP);

	m_pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", reinterpret_cast<const float*>(&m_LightVP));

	m_GameContext.pGame->SetRenderTarget(m_pShadowRenderTarget);

	m_pShadowRenderTarget->Clear();
}

void ShadowMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	ShadowGeneratorType curType{ pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static };
	
	const MaterialTechniqueContext& curTechniqueContext{ m_GeneratorTechniqueContexts[static_cast<int>(curType)] };

	m_pShadowMapGenerator->SetVariable_Matrix(L"gWorld", reinterpret_cast<const float*>(&meshWorld));
	if (pMeshFilter->HasAnimations())
		m_pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", reinterpret_cast<const float*>(meshBones.data()), static_cast<UINT>(meshBones.size()));

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(curTechniqueContext.pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		constexpr UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(curTechniqueContext.inputLayoutID, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);
		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_pShadowMapGenerator->SetTechnique(static_cast<int>(curType));
		D3DX11_TECHNIQUE_DESC techDesc{};
		curTechniqueContext.pTechnique->GetDesc(&techDesc);

		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			curTechniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapRenderer::End(const SceneContext&) const
{
	//1. Reset the Main Game RenderTarget back to default (OverlordGame::SetRenderTarget)
	m_GameContext.pGame->SetRenderTarget(nullptr);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
