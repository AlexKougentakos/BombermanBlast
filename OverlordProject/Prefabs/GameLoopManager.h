#pragma once

class BombermanCharacter;

enum class GamePhase
{
	PreRound,
	Round,
	RoundWithSkullBoxes,
	PostRound
};

class GameLoopManager : public GameObject, public Observer<BombermanCharacter>, public Subject<GameLoopManager>
{
public:
	GameLoopManager(const std::vector<BombermanCharacter*>& characters);
	virtual ~GameLoopManager() override = default;

	GameLoopManager(const GameLoopManager& other) = delete;
	GameLoopManager(GameLoopManager&& other) noexcept = delete;
	GameLoopManager& operator=(const GameLoopManager& other) = delete;
	GameLoopManager& operator=(GameLoopManager&& other) noexcept = delete;

	void IncreaseRoundNumber() { ++m_RoundNumber; }

	void DrawOnGUI();
	void StartGame() { notifyObservers("Pre-Round Start"); }

	std::map<int, int> GetPlayerScores() const { return m_PlayerScores; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

	void OnNotify(BombermanCharacter* source, const std::string& field) override;


private:
	/*
	 * TIMERS
	 */
	const float m_PreRoundTime{ 5.f }; //Seconds
	const float m_RoundTime{ 1.f }; //Seconds
	const float m_BoxDropDuration{ 43.f }; //Seconds
	const float m_PostRoundTime{ 5.f }; //Seconds

	float m_ElapsedRoundTime{0.f};

	int m_RoundNumber{ 1 };

	const std::vector<BombermanCharacter*>& m_pPlayers{};

	void SwitchToNextPhase();
	void SwitchToPreRound();
	void SwitchToRound();
	void SwitchToRoundWithSkullBoxes();
	void SwitchToPostRound();

	//Player index, score
	std::map<int, int> m_PlayerScores{};

	GamePhase m_GamePhase{ GamePhase::PreRound };
	SceneContext m_SceneContext{};
};
