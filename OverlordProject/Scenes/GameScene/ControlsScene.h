#pragma once

class ButtonManager;

class ControlsScene : public GameScene
{
public:
	ControlsScene() : GameScene(L"ControlsScene") {}
	~ControlsScene() override = default;

	ControlsScene(const ControlsScene& other) = delete;
	ControlsScene(ControlsScene&& other) noexcept = delete;
	ControlsScene& operator=(const ControlsScene& other) = delete;
	ControlsScene& operator=(ControlsScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void UpdateCustomCursor();
	void Update() override;

private:
	SpriteFont* m_pFont{};

	GameObject* m_SelectedButton{};

	GameObject* m_pCursor{};
	XMFLOAT2 m_GameCursorPosition{};

	ButtonManager* m_pButtonManager{};

	enum InputIds
	{
		ControllerMenuPress
	};

};
