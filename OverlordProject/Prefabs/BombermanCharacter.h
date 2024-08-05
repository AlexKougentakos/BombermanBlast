#pragma once
class ShadowCircleMaterial_Skinned;
class ShadowCircleMaterial;
class DiffuseMaterial_Shadow_Skinned;
class DiffuseMaterial_Skinned;
class GridComponent;

enum class PlayerColour
{
	WHITE = 1,
	BLUE = 2,
	RED = 3,
	YELLOW = 4
};

struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = 5.f,
		float height = 10.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 10.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ 10.f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_PlaceBomb{ -1 };
};

struct PlayerStatIncrease //To be used for PowerUps
{
	int bombs{};
	int blastRadius{};
	int speed{};
};

struct PlayerStats
{
	int maxBombs{ 1 };
	int blastRadius{ 1 };
	int speed{ 1 };

	PlayerStats& operator+= (const PlayerStatIncrease& increase)
	{
		maxBombs = std::max( maxBombs + increase.bombs, 1);
		blastRadius = std::max(blastRadius + increase.blastRadius, 1);
		speed = std::max(speed + increase.speed, 1);

		return *this;
	}
};

enum class PlayerState : UINT
{
	Idle = 0,
	Moving = 1,
	Dead = 2
};

class BombermanCharacter : public GameObject, public Subject<BombermanCharacter>
{
public:
	BombermanCharacter(const CharacterDesc& characterDesc, GridComponent* const pGrid);
	~BombermanCharacter() override = default;

	BombermanCharacter(const BombermanCharacter& other) = delete;
	BombermanCharacter(BombermanCharacter&& other) noexcept = delete;
	BombermanCharacter& operator=(const BombermanCharacter& other) = delete;
	BombermanCharacter& operator=(BombermanCharacter&& other) noexcept = delete;

	void DrawImGui();

	void GiveBackBomb();
	void SpawnBomb();
	void Kill();
	bool IsDead() const { return m_IsDead; }

	void ApplyPowerup(PlayerStatIncrease increase)
	{
		m_PlayerStats += increase;
		m_RemainingBombs = std::max(1, m_RemainingBombs + increase.bombs);
	}
	void AddPoint();
	PlayerColour GetPlayerColour() const
	{
		return PlayerColour(m_PlayerIndex);
	}

	void Reset();

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

	ModelAnimator* m_pModelAnimator{};
	ModelComponent* m_pModelComponent{};
	DiffuseMaterial_Skinned* m_pMaterial{};
	DiffuseMaterial_Skinned* m_pFaceMaterial{};
	UINT m_AnimationClipId{ 0 };

	PlayerStats m_PlayerStats{};
	GridComponent* m_pGrid{};

	PlayerState m_PlayerState{ PlayerState::Idle };

	const float m_DeathFlickerSpeed{ 10 }; //Flickers per second
	const float m_DeathAnimTime{ 1.f };
	float m_ElapsedDeathTimer{ 0.f };
	bool m_IsDead{ false };

	int m_RemainingBombs{ m_PlayerStats.maxBombs };

	void ChangeAnimationClip(UINT animationID);

	static int m_InstanceCounter;
	int m_PlayerIndex{0};
	int m_PlayerScore{0};
};