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

	//Initialize the map with the player index and their score
	for (const auto player : m_pPlayers)
	{
		m_PlayerScores[player->GetIndex()] = 0;
	}
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

	case GamePhase::RoundWithSkullBoxes:
		m_ElapsedRoundTime += sceneContext.pGameTime->GetElapsed();

		if (m_ElapsedRoundTime >= m_BoxDropDuration) SwitchToNextPhase();

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
		SwitchToRound();
		return;
	case GamePhase::Round:
		SwitchToRoundWithSkullBoxes();
		return;
	case GamePhase::RoundWithSkullBoxes:
		SwitchToPostRound();
		return;
	case GamePhase::PostRound:
		SwitchToPreRound();
		return;
	}
}

void GameLoopManager::SwitchToPreRound()
{
	m_SceneContext.pInput->SetEnabled(false);
	m_GamePhase = GamePhase::PreRound;
	m_ElapsedRoundTime = 0;
	
	notifyObservers("Pre-Round Start");

	//Brand new players are created so we need to register to their deaths again
	//This is done in the round start because the players are created in the pre-round
	for (BombermanCharacter* pPlayer : m_pPlayers)
	{
		pPlayer->registerObserver(this);
	}
}

void GameLoopManager::SwitchToRound()
{
	if (m_ElapsedRoundTime < m_PreRoundTime) return;

	m_GamePhase = GamePhase::Round;
	m_SceneContext.pInput->SetEnabled(true);
	m_ElapsedRoundTime = 0;
	
	notifyObservers("Round Start");
}

void GameLoopManager::SwitchToRoundWithSkullBoxes()
{
	m_GamePhase = GamePhase::RoundWithSkullBoxes;
	m_ElapsedRoundTime = 0;
	notifyObservers("Round-With-Skull-Boxes-Start");
}

void GameLoopManager::SwitchToPostRound()
{
	m_GamePhase = GamePhase::PostRound;
	m_ElapsedRoundTime = 0;
	m_SceneContext.pInput->SetEnabled(false);
	notifyObservers("Post-Round Start");
}

void GameLoopManager::OnNotify(BombermanCharacter* source, const std::string& field)
{
	if (field == "Player Death")
	{
		if (m_pPlayers.size() == 2)
		{
			//Whoever is NOT the source of the DEATH event gets a point
			if (m_pPlayers[0] == source) 
				m_pPlayers[1]->AddPoint();
			else m_pPlayers[0]->AddPoint();

			SwitchToPostRound();
		}
	}
	else if (field == "Score Increase")
	{
		const int currentScore = m_PlayerScores[source->GetIndex()];
		m_PlayerScores[source->GetIndex()] = currentScore + 1;
		notifyObservers("Player Score Increase");
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
