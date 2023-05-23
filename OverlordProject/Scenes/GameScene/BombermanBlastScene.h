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

protected:
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
		CharacterJump
	};

	void ChangeAnimationClip(UINT animationID);
	/*
	 This function spawns rocks on the map. It finds the bottom left and top right of the map and spawns rocks in between.
	 It scales the rocks and their colliders to fit to the map.
	 */
	void SpawnRocks();
	void ExplodeBomb(const BombPrefab& bomb);

	std::vector<char> m_Map
	{
		'X','O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X',
		'O','X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O',
		'X','O', 'X', 'O', 'X', 'O', 'O', 'O', 'X', 'O', 'X', 'O', 'X',
		'O','X', 'O', 'X', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'X', 'O',
		'X','O', 'X', 'O', 'X', 'O', 'O', 'O', 'X', 'O', 'X', 'O', 'X',
		'O','X', 'O', 'X', 'O', 'O', 'O', 'O', 'O', 'X', 'O', 'X', 'O',
		'X','O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X',
		'O','X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O',
		'X','O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X',
		'O','X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O',
	};

	XMFLOAT3 m_MapBottomLeft{}, m_MapTopRight{};
	float m_SingleBlockSize{};
	BombPrefab* test{};

	//Debug
	bool m_IsDebugCameraActive{ false };
};

