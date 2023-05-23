#pragma once

enum class RockType 
{
	BREAKABLE,
	UNBREAKABLE
};

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
	void Update(const SceneContext& gameContext) override;

	RockType GetType() const { return m_Type; }
	void MarkForDeletion() { m_ToBeDeleted = true; }

	float GetWidth() const { return m_pModelComponent->CalculateWidth(); }

private:
	RockType m_Type{};
	ModelComponent* m_pModelComponent{};
	bool m_ToBeDeleted{false};
	float m_Scale{};
};