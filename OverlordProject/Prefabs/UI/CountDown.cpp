#include "stdafx.h"
#include "CountDown.h"

CountDown::CountDown(float timeInSeconds, const XMFLOAT2& position) :
	m_TimeInSeconds(timeInSeconds),
	m_Position(position)
{

}

void CountDown::Initialize(const SceneContext&) 
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./SpriteFonts/Bomberman.fnt");
}

void CountDown::Update(const SceneContext& sceneContext)
{
	if (!m_CountdownActive) return;

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(std::to_string(
		static_cast<int>(m_TimeInSeconds - static_cast<int>(m_ElapsedTime)))), m_Position);

	m_ElapsedTime += sceneContext.pGameTime->GetElapsed();

	if (m_ElapsedTime >= m_TimeInSeconds)
	{
		m_CountdownActive = false;
	}
}
