#pragma once
class BombermanCharacter;

class CountDown : public GameObject
{
public:
	CountDown(float timeInSeconds, const XMFLOAT2& position);
	virtual ~CountDown() override = default;

	CountDown(const CountDown& other) = delete;
	CountDown(CountDown&& other) noexcept = delete;
	CountDown& operator=(const CountDown& other) = delete;
	CountDown& operator=(CountDown&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;

	void StartCountDown();
	void SetActive(const bool active) { m_CountdownActive = active; }

protected:
	virtual void Update(const SceneContext& sceneContext) override;
private:
	float m_ElapsedTime{ 0 };
	const float m_TimeInSeconds{ 0 };

	bool m_CountdownActive{ false };
	bool m_CountdownFinished{ false };

	XMFLOAT2 m_Position{};
	SpriteFont* m_pFont{};

};

