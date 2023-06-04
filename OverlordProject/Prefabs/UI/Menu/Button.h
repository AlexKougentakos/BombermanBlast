#pragma once
class Button : public GameObject
{
public:
	Button(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~Button() override = default;
	
	Button(const Button& other) = delete;
	Button(Button&& other) noexcept = delete;
	Button& operator=(const Button& other) = delete;
	Button& operator=(Button&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;
	void PostInitialize(const SceneContext& sceneContext) override;
	float GetWidth() const { return m_pSpriteComponent->GetDimensions().x; }
	float GetHeight() const { return m_pSpriteComponent->GetDimensions().y; }

	/**
	 * \brief Triggers on click. Works with both mouse buttons
	 */
	virtual void OnClick() const {}
	/**
	 * \brief Triggers only once when the mouse is hovering over the button
	 */
	virtual void OnHover() const {}
	/**
	 * \brief Triggers only once when the mouse was hovering over the button but is not anymore
	 */
	virtual void OnHoverExit() const {}

	void SetIsHovering(bool isHovering) { m_IsHovering = isHovering; }
	bool IsHovering() const { return m_IsHovering; }

protected:


	XMFLOAT2 m_Position{};
	SpriteComponent* m_pSpriteComponent{};

	bool m_IsHovering{ false };
};

