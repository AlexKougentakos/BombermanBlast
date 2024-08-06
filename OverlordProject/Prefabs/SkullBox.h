#pragma once

class GridComponent;

class SkullBox final : public GameObject
{
public:
	SkullBox(const float scale, GridComponent* pGridComponent);
	virtual ~SkullBox() override = default;

	SkullBox(const SkullBox& other) = delete;
	SkullBox(SkullBox&& other) noexcept = delete;
	SkullBox& operator=(const SkullBox& other) = delete;
	SkullBox& operator=(SkullBox&& other) noexcept = delete;

	void Initialize(const SceneContext& gameContext) override;
	void Update(const SceneContext&) override;

	//Call this after they have been placed on the grid
	void PlacedInGrid();
private:
	ModelComponent* m_pModelComponent{};
	GridComponent* m_pGridComponent{};
	float m_Scale{};
	float m_FallingSpeed{ 10.f };

	bool m_OnGround {false};
	bool m_InitializedCollision{false};
};
