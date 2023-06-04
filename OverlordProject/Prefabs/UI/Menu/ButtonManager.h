#pragma once
class Button;

class ButtonManager final : public GameObject
{
public:
	ButtonManager();
	virtual ~ButtonManager() override = default;

	ButtonManager(const ButtonManager& other) = delete;
	ButtonManager(ButtonManager&& other) noexcept = delete;
	ButtonManager& operator=(const ButtonManager& other) = delete;
	ButtonManager& operator=(ButtonManager&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;
	virtual void Update(const SceneContext&) override;

	void AddButton(Button* pButton);

	void UpdateButtons(const XMFLOAT2& mousePos);
private:
	std::vector<Button*> m_pButtons{};

	bool m_ExecutedOnHoverEnter{ false };
	bool m_ExecutedOnHoverExit{ false };
};
