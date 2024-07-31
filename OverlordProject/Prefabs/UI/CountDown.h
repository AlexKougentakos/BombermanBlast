#pragma once
class BombermanCharacter;

class CountDown : public GameObject, public Subject<CountDown>
{
public:
	CountDown(float timeInSeconds, const XMFLOAT2& position);
	virtual ~CountDown() override = default;

	CountDown(const CountDown& other) = delete;
	CountDown(CountDown&& other) noexcept = delete;
	CountDown& operator=(const CountDown& other) = delete;
	CountDown& operator=(CountDown&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;

	void StartCountDown() { m_CountdownActive = true; m_ElapsedTime = 0; }

protected:
	virtual void Update(const SceneContext& sceneContext) override;
private:
	float m_ElapsedTime{ 0 };
	const float m_TimeInSeconds{ 0 };

	bool m_CountdownActive{ false };

	XMFLOAT2 m_Position{};
	SpriteFont* m_pFont{};

};

