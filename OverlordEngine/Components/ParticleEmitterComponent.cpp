#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//We only create one instance of it
	if (!m_pParticleMaterial)
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_ParticleCount * static_cast<UINT>(sizeof(VertexParticle));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	if (sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer) != S_OK)
			Logger::LogError(L"ParticleEmitterComponent::CreateVertexBuffer() - Error when creating m_pVertexBuffer!");
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{

	const float averageEnergy{ m_EmitterSettings.maxEnergy - m_EmitterSettings.minEnergy };
	const float particleInterval = averageEnergy / float(m_ParticleCount);

	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	D3D11_BUFFER_DESC mapDesc{};
	m_pVertexBuffer->GetDesc(&mapDesc);

	D3D11_MAPPED_SUBRESOURCE mappedSubResource{ nullptr };
	if (sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) != S_OK)
		Logger::LogError(L"ParticleEmitterComponent::Update() - Error when mapping m_pVertexBuffer!");

	VertexParticle* pBuffer{ nullptr };
	pBuffer = static_cast<VertexParticle*>(mappedSubResource.pData);

	for (size_t idx{}; idx < m_ParticleCount; ++idx)
	{
		if (m_ParticlesArray[idx].isActive)
			UpdateParticle(m_ParticlesArray[idx], sceneContext.pGameTime->GetElapsed());

		if (!m_ParticlesArray[idx].isActive && m_LastParticleSpawn >= particleInterval)
			SpawnParticle(m_ParticlesArray[idx]);

		if (m_ParticlesArray[idx].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[idx].vertexInfo;
			++m_ActiveParticles;
		}

	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	if (!p.isActive)
		return;

	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0.f)
	{
		p.isActive = false;
		return;
	}

	p.vertexInfo.Position.x += (m_EmitterSettings.velocity.x * elapsedTime);
	p.vertexInfo.Position.y += (m_EmitterSettings.velocity.y * elapsedTime);
	p.vertexInfo.Position.z += (m_EmitterSettings.velocity.z * elapsedTime);

	const float lifePercent = p.currentEnergy / p.totalEnergy;

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w = m_EmitterSettings.color.w * lifePercent * 2.f;

	//Lerp
	const float min{ p.initialSize };
	const float max{ p.sizeChange + min };
	const float t{ 1.f - lifePercent };
	p.vertexInfo.Size = min + t * (max - min);
}

void ParticleEmitterComponent::SpawnParticle(Particle& p) const
{
	p.isActive = true;
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	XMVECTOR randomDirection = { 1.f, 0.f, 0.f };
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	randomDirection = XMVector3TransformNormal(randomDirection, rotationMatrix);

	const float randomDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	const XMVECTOR EmitterPos = XMLoadFloat3(&GetTransform()->GetPosition());

	XMStoreFloat3(&(p.vertexInfo.Position), EmitterPos + (randomDirection * randomDistance));

	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = p.vertexInfo.Size;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	const MaterialTechniqueContext techniqueContext = m_pParticleMaterial->GetTechniqueContext();

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	constexpr UINT offset{ 0 };
	constexpr UINT stride{ sizeof(VertexParticle) };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	techniqueContext.pTechnique->GetDesc(&techniqueDesc);

	for (uint32_t passIdx{}; passIdx < techniqueDesc.Passes; ++passIdx)
	{
		techniqueContext.pTechnique->GetPassByIndex(passIdx)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}