#include "stdafx.h"
#include "Timer.h"

Timer::Timer(float timeInSeconds, const XMFLOAT2& position)
	:m_TimeInSeconds(timeInSeconds),
	m_Position(position)
{
	
}


void Timer::Initialize(const SceneContext&)
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./SpriteFonts/Thin_Font.fnt");
	m_pSpriteComponent = AddComponent(new SpriteComponent(L"Textures/UI/TimerBG.png"));

	m_pSpriteComponent->GetTransform()->Translate( m_Position.x - 15.f, m_Position.y, 0.2f);
}

std::string Timer::FormatTime(int timeInSeconds) const
{
	const int minutes = timeInSeconds / 60;
	const int seconds = timeInSeconds % 60;

	std::stringstream ss;
	ss << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
	return ss.str();
}

void Timer::Update(const SceneContext& sceneContext)
{
	

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(FormatTime(static_cast<int>(m_TimeInSeconds - m_ElapsedTime))), m_Position);

	if (!m_StartTimer && m_ElapsedTime < m_TimeInSeconds) return;

	m_ElapsedTime += sceneContext.pGameTime->GetElapsed();

	if (m_ElapsedTime >= m_TimeInSeconds)
	{
		m_ElapsedTime = 0;
	}
}

