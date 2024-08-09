#pragma once

class ButtonManager;

class MainMenuScene : public GameScene
{
public:
	MainMenuScene();
	~MainMenuScene() override = default;

	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void UpdateCustomCursor();
	void Update() override;
	void OnSceneActivated() override;

private:
	GameObject* m_pCursor{};
	XMFLOAT2 m_GameCursorPosition{};

	ButtonManager* m_pButtonManager{};
};
