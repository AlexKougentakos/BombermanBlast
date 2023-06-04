#pragma once
#include "Prefabs/UI/Menu/Button.h"

class QuitButton final : public Button
{
public:
	QuitButton(const XMFLOAT2& pos, const std::wstring& coverImagePath);
	virtual ~QuitButton() override = default;

	QuitButton(const QuitButton& other) = delete;
	QuitButton(QuitButton&& other) noexcept = delete;
	QuitButton& operator=(const QuitButton& other) = delete;
	QuitButton& operator=(QuitButton&& other) noexcept = delete;

	virtual void OnClick() const override;
	virtual void OnHover() const override;
	virtual void OnHoverExit() const override;

private:
	XMFLOAT4 m_NormalColour{ Colors::White };
	XMFLOAT4 m_HoverColour{ Colors::White };

	bool m_IsHovering{ false };
};
