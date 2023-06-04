#pragma once
#include "Prefabs/UI/Menu/Button.h"

class BackButton final : public Button
{
public:
	BackButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~BackButton() override = default;

	BackButton(const BackButton& other) = delete;
	BackButton(BackButton&& other) noexcept = delete;
	BackButton& operator=(const BackButton& other) = delete;
	BackButton& operator=(BackButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
