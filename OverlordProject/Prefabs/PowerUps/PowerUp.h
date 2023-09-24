#pragma once


struct GridCell;
class DiffuseMaterial_Shadow;
class GridComponent;

class BasePowerUp : public GameObject
{
public:
	virtual ~BasePowerUp() override = default;

	BasePowerUp(const BasePowerUp& other) = delete;
	BasePowerUp(BasePowerUp&& other) noexcept = delete;
	BasePowerUp& operator=(const BasePowerUp& other) = delete;
	BasePowerUp& operator=(BasePowerUp&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;
	bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
	void MarkForRemoval() { m_MarkedForRemoval = true; }

	void SetGridCell(GridCell* pGridCell) { m_pGridCell = pGridCell; }


private:
	GridComponent* m_pGrid{};
	ModelComponent* m_pModelComponent{};
	DiffuseMaterial_Shadow* m_pMaterial{};
	std::wstring m_TexturePath{};
	bool m_MarkedForRemoval{ false };
	RigidBodyComponent* m_pRigidBody{};
	GridCell* m_pGridCell{};
protected:
	BasePowerUp(GridComponent* pGridComponent, GridCell* pGridCell);
	void SetTexture(const std::wstring& texturePath);
	void Explode(GridCell* gridCell) const;

	void OnCollisionRoot(GameObject* trigger, GameObject* other, PxTriggerAction action);
	virtual void OnCollision(GameObject* trigger, GameObject* other, PxTriggerAction action) = 0;
};


