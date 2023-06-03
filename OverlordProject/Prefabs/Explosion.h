#pragma once

class GridComponent;

class Explosion final : public GameObject
{
public:
	Explosion(GridComponent* grid);
	virtual ~Explosion() override = default;

	Explosion(const Explosion& other) = delete;
	Explosion(Explosion&& other) noexcept = delete;
	Explosion& operator=(const Explosion& other) = delete;
	Explosion& operator=(Explosion&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
private:
	float m_ExplosionDuration{ 1.f };
	float m_ExplosionElapsedTime{ 0.f };

	//We need access to the grid to remove the explosion from the grid
	GridComponent* m_pGrid{};
	ParticleEmitterComponent* m_pEmitter{};
};
