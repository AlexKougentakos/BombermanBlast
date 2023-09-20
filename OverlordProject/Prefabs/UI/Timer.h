#pragma once

class BombermanCharacter;
enum class PlayerColour;

class Timer : public GameObject, public Subject<Timer>
{
public:
	Timer(float timeInSeconds, const XMFLOAT2& position);
	virtual ~Timer() override = default;

	Timer(const Timer& other) = delete;
	Timer(Timer&& other) noexcept = delete;
	Timer& operator=(const Timer& other) = delete;
	Timer& operator=(Timer&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;

	void StartTimer() { m_StartTimer = true; }
	void ResetTimer() { m_ElapsedTime = 0; m_StartTimer = false; }
	void ZeroTimer() { m_ElapsedTime = m_TimeInSeconds; m_StartTimer = false; }

protected:
	virtual void Update(const SceneContext& sceneContext) override;
private:
	float m_ElapsedTime{0};
	const float m_TimeInSeconds{0};

	bool m_StartTimer{ false };

	std::string FormatTime(int timeInSeconds) const;

	XMFLOAT2 m_Position{};
	SpriteFont* m_pFont{};
	SpriteComponent* m_pSpriteComponent{};

};
