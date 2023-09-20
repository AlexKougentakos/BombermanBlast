#pragma once
#include "Prefabs/UI/Menu/Button.h"

class MenuButton final : public Button
{
public:
	MenuButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~MenuButton() override = default;

	MenuButton(const MenuButton& other) = delete;
	MenuButton(MenuButton&& other) noexcept = delete;
	MenuButton& operator=(const MenuButton& other) = delete;
	MenuButton& operator=(MenuButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
