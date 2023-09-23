#include "stdafx.h"
#include "Explosion.h"

#include "Components/Grid.h"

Explosion::Explosion(GridComponent* grid):
	m_pGrid(grid)
{
	SetTag(L"Explosion");
}

void Explosion::Initialize(const SceneContext&)
{
	const float scale = m_pGrid->GetScaleFactor();

	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,0.f,0.f };
	settings.minSize = 0.75f * scale;
	settings.maxSize = 1.f * scale;
	settings.minEnergy = m_ExplosionDuration;
	settings.maxEnergy = m_ExplosionDuration;
	settings.minScale = .2f;
	settings.maxScale = .5f;
	settings.minEmitterRadius = .02f;
	settings.maxEmitterRadius = .08f;
	settings.color = { 1.f,1.f,1.f, .6f };

	m_pEmitter = AddComponent(new ParticleEmitterComponent(L"Textures/FireBall.png", settings, 
50));
}

void Explosion::Update(const SceneContext& sceneContext)
{
	m_ExplosionElapsedTime += sceneContext.pGameTime->GetElapsed();

	if (m_ExplosionElapsedTime >= m_ExplosionDuration)
	{
		m_pGrid->RemoveObject(this);
	}
}
