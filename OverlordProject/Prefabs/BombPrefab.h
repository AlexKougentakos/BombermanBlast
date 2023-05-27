#pragma once

struct GridCell;
class GridComponent;

class BombPrefab final : public GameObject
{
public:
	BombPrefab(int blastRadius, GridComponent* pGridComponent);
	virtual ~BombPrefab() override = default;

	BombPrefab(const BombPrefab& other) = delete;
	BombPrefab(BombPrefab&& other) noexcept = delete;
	BombPrefab& operator=(const BombPrefab& other) = delete;
	BombPrefab& operator=(BombPrefab&& other) noexcept = delete;

	void Initialize(const SceneContext& gameContext) override;

	float GetWidth() const { return m_pModelComponent->CalculateWidth(); }
protected:
	virtual void Update(const SceneContext& sceneContext) override;

private:
	ModelComponent* m_pModelComponent{};

	const float m_ExplosionTime{ 3.f };
	float m_FuseElapsedTime{0.f};
	int m_BlastRadius{ 1 };

	void Explode(int explosionDistance) const;
	void ExplodeRecursive(const GridCell& cell, int explosionDistance, std::vector<GridCell>& affectedCellsOut) const;
	GridComponent* m_pGrid{};
};
