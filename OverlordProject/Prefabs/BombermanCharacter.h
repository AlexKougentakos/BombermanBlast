#pragma once
class GridComponent;

struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 1.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 4.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_PlaceBomb{ -1 };
};

struct PlayerStats
{
	int bombs{ 1 };
	int blastRadius{ 3 };
};

class BombermanCharacter : public GameObject
{
public:
	BombermanCharacter(const CharacterDesc& characterDesc, GridComponent* const pGrid);
	~BombermanCharacter() override = default;

	BombermanCharacter(const BombermanCharacter& other) = delete;
	BombermanCharacter(BombermanCharacter&& other) noexcept = delete;
	BombermanCharacter& operator=(const BombermanCharacter& other) = delete;
	BombermanCharacter& operator=(BombermanCharacter&& other) noexcept = delete;

	void DrawImGui();
	float GetYAxisRotation() const { return m_YRotation; }

	void SpawnBomb() const;

protected:
	void Initialize(const SceneContext&) override;
	void HandleInputAndMovement(const SceneContext& sceneContext);
	void Update(const SceneContext&) override;

private:
	ControllerComponent* m_pControllerComponent{};

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
	float m_YRotation{}; //In Degrees

	PlayerStats m_PlayerStats{};
	GridComponent* m_pGrid{};
};

