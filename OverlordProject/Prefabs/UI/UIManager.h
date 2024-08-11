#pragma once
class RoundWinners;
class CountDown;
class BombermanCharacter;
class GameLoopManager;

class Timer;

class UIManager : public GameObject, public Observer<BombermanCharacter>
{
public:
	UIManager(std::vector<BombermanCharacter*> players, GameLoopManager* pGameLoopManager);
	virtual ~UIManager() override = default;

	UIManager(const UIManager& other) = delete;
	UIManager(UIManager&& other) noexcept = delete;
	UIManager& operator=(const UIManager& other) = delete;
	UIManager& operator=(UIManager&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;
	void OnNotify(BombermanCharacter* source, const std::string& field) override;
	void Update(const SceneContext&) override;

	void StartTimer() const;
	void ResetTimer() const;
	void ZeroTimer() const;
	void StartCountdown() const;

	void UpdatePlayers(const std::vector<BombermanCharacter*>& players);
	void ShowRoundWinners();
	void HideRoundWinners();

private:
	std::vector<BombermanCharacter*> m_pPlayers{};
	GameLoopManager* m_pGameLoopManager{ nullptr };
	Timer* m_pTimer{ nullptr };
	CountDown* m_pCountdown{ nullptr };
	RoundWinners* m_pRoundWinners{ nullptr };

	bool m_ShowingPauseMenu{ false };

	GameObject* m_pPauseMenuContainer{};
	GameObject* m_pCursor{};
	XMFLOAT2 m_GameCursorPosition{};
	ButtonManager* m_pButtonManager{};

	XMFLOAT2 m_MenuButtonPos{};
	XMFLOAT2 m_QuitButtonPos{};
	
	void AddPlayerHeads(const SceneContext& sceneContext);
	void ShowPauseMenu();
	void AddTimer(const SceneContext& sceneContext);
};
