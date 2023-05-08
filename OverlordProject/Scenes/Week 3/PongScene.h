#pragma once
class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:

	void Reset() const;

	GameObject* m_pPaddleLeft{ nullptr };
	GameObject* m_pPaddleRight{ nullptr };
	GameObject* m_pBall{ nullptr };

	CameraComponent* m_pCamera{nullptr};

	float m_PaddleSpeed{ 25.f };
	float m_BallSpeed{ 20.f };

};


