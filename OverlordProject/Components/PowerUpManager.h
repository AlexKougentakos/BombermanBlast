#pragma once
class GridComponent;
struct GridCell;
class BombermanCharacter;
class BasePowerUp;

enum class PowerUpType
{
	BombUp = 1,
	BombDown = 2,
	FlameUp = 3,
	FlameDown = 4,
	SpeedUp = 5,
	SpeedDown = 6
};

class PowerUpManager final : public BaseComponent
{
public:
	PowerUpManager(GridComponent* pGridComponent);
	~PowerUpManager() override = default;

	PowerUpManager(const PowerUpManager& other) = delete;
	PowerUpManager(PowerUpManager&& other) noexcept = delete;
	PowerUpManager& operator=(const PowerUpManager& other) = delete;
	PowerUpManager& operator=(PowerUpManager&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;

	void TriggerPowerUpSpawn(GridCell& cell);
	BasePowerUp* GetPowerUpByType(PowerUpType type, GridCell* pGridCell) const;
	void RemovePowerUp(BasePowerUp* pPowerUp);

	void UpdatePowerUps();
private:
	void AddPowerUp(BasePowerUp* pPowerUp, GridCell& cell);
	std::vector<BasePowerUp*> m_pPowerUps{};

	GridComponent* m_pGrid{};

	float m_SpawnChance{40};
	int m_NumOfPowerUps{6};
};
