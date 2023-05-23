#pragma once


class BombPrefab final : public GameObject
{
public:
	BombPrefab();
	virtual ~BombPrefab() override = default;

	BombPrefab(const BombPrefab& other) = delete;
	BombPrefab(BombPrefab&& other) noexcept = delete;
	BombPrefab& operator=(const BombPrefab& other) = delete;
	BombPrefab& operator=(BombPrefab&& other) noexcept = delete;

	void Initialize(const SceneContext& gameContext) override;
	void Update(const SceneContext& gameContext) override;

private:
	ModelComponent* m_pModelComponent{};
};