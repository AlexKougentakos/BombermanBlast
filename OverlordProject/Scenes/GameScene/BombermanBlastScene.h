#pragma once
#include "Prefabs/CubePrefab.h"
class SkullBox;
class MenuButton;
class QuitButton;
class GameLoopManager;
class BombPrefab;
class BombermanCharacter;
class UIManager;

class BombermanBlastScene final : public GameScene, public Observer<GameLoopManager>
{
public:
	BombermanBlastScene();
	~BombermanBlastScene() override;

	BombermanBlastScene(const BombermanBlastScene& other) = delete;
	BombermanBlastScene(BombermanBlastScene&& other) noexcept = delete;
	BombermanBlastScene& operator=(const BombermanBlastScene& other) = delete;
	BombermanBlastScene& operator=(BombermanBlastScene&& other) noexcept = delete;

	GameObject* GetLevel() const { return m_pLevel; }
	void OnSceneActivated() override;

protected:
	void CalculateNeededBlockSize();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	void PostInitialize() override;
	virtual void OnNotify(GameLoopManager* source, const std::string& event) override;

private:
	//Characters
	std::vector<BombermanCharacter*> m_pCharacters{};
	const short unsigned int m_NumOfPlayers{4};

	std::vector<SkullBox*> m_pSkullBoxes{};

	//Cameras
	FixedCamera* m_pMainCamera{ nullptr };
	FreeCamera* m_pDebugCamera{ nullptr };


	enum InputIds
	{
		CharacterMoveLeft_P1,
		CharacterMoveRight_P1,
		CharacterMoveForward_P1,
		CharacterMoveBackward_P1,
		CharacterPlaceBomb_P1,

		CharacterMoveLeft_P2,
		CharacterMoveRight_P2,
		CharacterMoveForward_P2,
		CharacterMoveBackward_P2,
		CharacterPlaceBomb_P2,

		CharacterMoveLeft_P3,
		CharacterMoveRight_P3,
		CharacterMoveForward_P3,
		CharacterMoveBackward_P3,
		CharacterPlaceBomb_P3,

		CharacterMoveLeft_P4,
		CharacterMoveRight_P4,
		CharacterMoveForward_P4,
		CharacterMoveBackward_P4,
		CharacterPlaceBomb_P4,
	};

	/*
	 This function spawns rocks on the map. It finds the bottom left and top right of the map and spawns rocks in between.
	 It scales the rocks and their colliders to fit to the map.
	 */
	void SpawnRocks() const;
	void AddCharacters(PxMaterial* pDefaultMaterial, int numOfPlayers);
	void InitializeLevel(const PxMaterial* pDefaultMaterial);
	void DefinePlayerInputs() const;
	void InitializeLevelNecessities();
	void ResetLevel();

	//This will appear reversed in the game. The topLeft is the bottom left..etc
	//Map is to be used for debug visualizations
	std::vector<char> m_Map{};
	std::vector<char> m_StartingLayout
	{
		'O','O', 'O', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'O', 'O', 'O',
		'O','S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'O',
		'O','R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'O',
		'R','S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R',
		'R','R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
		'R','S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R',
		'R','R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
		'R','S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R',
		'O','R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'O',
		'O','S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'R', 'S', 'O',
		'O','O', 'O', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'O', 'O', 'O',
	};

	// Referring to the map above
	int m_NumOfColumns{13}, m_NumOfRows{11};

	//World
	GameObject* m_pLevel{};

	XMFLOAT3 m_MapBottomLeft{}, m_MapTopRight{};
	float m_SingleBlockSize{};
	float m_SingleBlockScale{};

	ParticleEmitterComponent* m_pEmitter{};

	//Skull boxes
	const float m_BoxDropDuration{ 20.f };
	const float m_BoxDropInterval{ 0.2f };
	float m_ElapsedDroppingTime{ 0.f };
	bool m_StartedDropping{ false };
	int m_CurrentIndex{ 0 };

	//Debug
	bool m_IsDebugCameraActive{ false };
	PxVec3 m_OnGuiCameraRotation{0.f, 0.f, 0.f};

	GameLoopManager* m_pGameLoopManager{ nullptr };
	UIManager* m_pUIManager{ nullptr };

	bool m_IsGameOver{ false };
};