#pragma once
#include "Prefabs/UI/Menu/Button.h"

class ControllButton final : public Button
{
public:
	ControllButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~ControllButton() override = default;

	ControllButton(const ControllButton& other) = delete;
	ControllButton(ControllButton&& other) noexcept = delete;
	ControllButton& operator=(const ControllButton& other) = delete;
	ControllButton& operator=(ControllButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
