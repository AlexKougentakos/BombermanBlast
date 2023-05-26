#pragma once
#include "Prefabs/CubePrefab.h"
class BombPrefab;
class BombermanCharacter;

class BombermanBlastScene final : public GameScene
{
public:
	BombermanBlastScene();
	~BombermanBlastScene() override = default;

	BombermanBlastScene(const BombermanBlastScene& other) = delete;
	BombermanBlastScene(BombermanBlastScene&& other) noexcept = delete;
	BombermanBlastScene& operator=(const BombermanBlastScene& other) = delete;
	BombermanBlastScene& operator=(BombermanBlastScene&& other) noexcept = delete;

	GameObject* GetLevel() const { return m_pLevel; }

protected:
	void CalculateNeededBlockSize();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:

	//Character
	BombermanCharacter* m_pCharacter{};

	//Animations
	ModelAnimator* m_pModelAnimator{};
	ModelComponent* m_pModelComponent{};
	UINT m_AnimationClipId{ 0 };

	//World
	GameObject* m_pLevel{};

	//Cameras
	CameraComponent* m_pCamera{ nullptr };
	FixedCamera* m_pMainCamera{ nullptr };
	FreeCamera* m_pDebugCamera{ nullptr };


	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb
	};

	void ChangeAnimationClip(UINT animationID);
	/*
	 This function spawns rocks on the map. It finds the bottom left and top right of the map and spawns rocks in between.
	 It scales the rocks and their colliders to fit to the map.
	 */
	void SpawnRocks();
	void ExplodeBomb(const BombPrefab& bomb);

	//This will appear reversed in the game. The topLeft is the bottom left..etc
	std::vector<char> m_Map
	{
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
		'O','O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
	};

	int m_NumOfColumns{13}, m_NumOfRows{10};

	XMFLOAT3 m_MapBottomLeft{}, m_MapTopRight{};
	float m_SingleBlockSize{};
	float m_SingleBlockScale{};

	//Debug
	bool m_IsDebugCameraActive{ false };
	GameObject* ptest{};
};

