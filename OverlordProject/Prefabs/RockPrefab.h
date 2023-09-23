#pragma once

enum class RockType 
{
	BREAKABLE,
	UNBREAKABLE
};

struct GridCell;

class RockPrefab final : public GameObject
{
public:
	RockPrefab(RockType type, float scale = 1.f);
	virtual ~RockPrefab() override = default;

	RockPrefab(const RockPrefab& other) = delete;
	RockPrefab(RockPrefab&& other) noexcept = delete;
	RockPrefab& operator=(const RockPrefab& other) = delete;
	RockPrefab& operator=(RockPrefab&& other) noexcept = delete;

	void Initialize(const SceneContext& gameContext) override;
	void Update(const SceneContext&) override {};

	RockType GetType() const { return m_Type; }
	void Explode() const;

	float GetWidth() const { return m_pModelComponent->CalculateWidth(); }
	void AssignCell(GridCell* pCell) { m_pCurrentCell = pCell; }

private:
	RockType m_Type{};
	ModelComponent* m_pModelComponent{};
	float m_Scale{};

	GridCell* m_pCurrentCell{};
};