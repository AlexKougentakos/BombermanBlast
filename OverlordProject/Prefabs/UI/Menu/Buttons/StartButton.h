#pragma once
#include "Prefabs/UI/Menu/Button.h"

class StartButton final : public Button
{
public:
	StartButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~StartButton() override = default;

	StartButton(const StartButton& other) = delete;
	StartButton(StartButton&& other) noexcept = delete;
	StartButton& operator=(const StartButton& other) = delete;
	StartButton& operator=(StartButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
