#pragma once
#include "Prefabs/UI/Menu/Button.h"

class ContinueButton : public Button
{
public:
	ContinueButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~ContinueButton() override = default;

	ContinueButton(const ContinueButton& other) = delete;
	ContinueButton(ContinueButton&& other) noexcept = delete;
	ContinueButton& operator=(const ContinueButton& other) = delete;
	ContinueButton& operator=(ContinueButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
