#pragma once
class Button;

class ButtonManager final : public GameObject
{
public:
	ButtonManager(const XMFLOAT2* pCustomCursor = nullptr);
	virtual ~ButtonManager() override = default;

	ButtonManager(const ButtonManager& other) = delete;
	ButtonManager(ButtonManager&& other) noexcept = delete;
	ButtonManager& operator=(const ButtonManager& other) = delete;
	ButtonManager& operator=(ButtonManager&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;
	virtual void Update(const SceneContext&) override;

	void AddButton(Button* pButton);

	void UpdateButtons(const XMFLOAT2& mousePos, const XMFLOAT2& customCursor = {-1, -1});
private:
	std::vector<Button*> m_pButtons{};

	bool m_ExecutedOnHoverEnter{ false };
	bool m_ExecutedOnHoverExit{ false };

	bool m_UsingCustomCursor{false};
	const XMFLOAT2* m_pCustomCursorPos;

	SceneContext m_SceneContext{};
	int m_ControllerClickId{};
};
