#include "stdafx.h"
#include "GameLoopManager.h"

#include "BombermanCharacter.h"

GameLoopManager::GameLoopManager(const std::vector<BombermanCharacter*>& characters):
	m_pPlayers(characters)
{
	for (BombermanCharacter* pPlayer : m_pPlayers)
	{
		pPlayer->registerObserver(this);
	}
}

void GameLoopManager::Initialize(const SceneContext& sceneContext)
{
	m_SceneContext = sceneContext;
}

void GameLoopManager::Update(const SceneContext& sceneContext)
{
	switch(m_GamePhase)
	{
	case GamePhase::PreRound:
		m_ElapsedRoundTime += sceneContext.pGameTime->GetElapsed();

		if (m_ElapsedRoundTime >= m_PreRoundTime) SwitchToNextPhase();
		break;

	case GamePhase::Round:
		m_ElapsedRoundTime += sceneContext.pGameTime->GetElapsed();

		if (m_ElapsedRoundTime >= m_RoundTime) SwitchToNextPhase();

		break;
	case GamePhase::PostRound:
		m_ElapsedRoundTime += sceneContext.pGameTime->GetElapsed();

		if (m_ElapsedRoundTime >= m_PostRoundTime) SwitchToNextPhase();
		break;
	}
}

void GameLoopManager::SwitchToNextPhase()
{
	switch (m_GamePhase)
	{
		//TODO: Fix weird bug in the timings of the phases. it switches from the pre-round to the round too quickly
	case GamePhase::PreRound:
		if (m_ElapsedRoundTime < m_PreRoundTime) return;

		m_GamePhase = GamePhase::Round;
		m_SceneContext.pInput->SetEnabled(true);
		m_ElapsedRoundTime = 0;
		notifyObservers("Round Start");
		return;

	case GamePhase::Round:
		if (m_pPlayers.size() > 1)
		for (const auto& player : m_pPlayers)
		{
			if (player->IsDead()) return;

			player->AddPoint();
		}

		m_SceneContext.pInput->SetEnabled(false);
		m_GamePhase = GamePhase::PostRound;
		m_ElapsedRoundTime = 0;
		notifyObservers("Post-Round Start");
		return;

	case GamePhase::PostRound:
		m_SceneContext.pInput->SetEnabled(false);
		m_GamePhase = GamePhase::PreRound;
		m_ElapsedRoundTime = 0;
		notifyObservers("Pre-Round Start");
		return;
	}
}

void GameLoopManager::OnNotify(BombermanCharacter* source, const std::string& field)
{
	if (field == "Player Death")
	{
		if (m_pPlayers.size() == 2)
		{
			//Whoever is not the source of the DEATH event gets a point
			if (m_pPlayers[0] == source) 
				m_pPlayers[1]->AddPoint();
			else m_pPlayers[0]->AddPoint();

			m_GamePhase = GamePhase::PostRound;
		}
	}
}

void GameLoopManager::DrawOnGUI()
{
	if (ImGui::Button("Pre-Round"))
	{
		m_GamePhase = GamePhase::PreRound;
		m_SceneContext.pInput->SetEnabled(false);
		notifyObservers("Pre-Round Start");
	}
	if (ImGui::Button("Round"))
	{
		m_GamePhase = GamePhase::Round;
		notifyObservers("Round Start");
	}
	if (ImGui::Button("End Round"))
	{
		m_GamePhase = GamePhase::PostRound;
		notifyObservers("Post-Round Start");
	}
}
