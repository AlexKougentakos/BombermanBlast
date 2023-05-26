#include "stdafx.h"
#include "BombermanCharacter.h"
#include "Scenes/GameScene/BombermanBlastScene.h"
#include "Components/Grid.h"
#include "Prefabs/BombPrefab.h"

BombermanCharacter::BombermanCharacter(const CharacterDesc& characterDesc, GridComponent* const pGrid) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_pGrid(pGrid)
{}

void BombermanCharacter::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	const auto controller = m_pControllerComponent->GetPxController();

	
	controller->setPosition({ 0,-10, 0 });
	m_pControllerComponent->SetContactOffset(0.0001f);
	m_pControllerComponent->SetStepOffset(0);

	SetTag(L"Player");
}

void BombermanCharacter::Update(const SceneContext& sceneContext)
{
	if (!sceneContext.settings.inDebug)
	{
		HandleInputAndMovement(sceneContext);
	}
}

void BombermanCharacter::SpawnBomb() const
{
 	const XMFLOAT3 playerPos = GetTransform()->GetPosition();
	const GridCell playerCell = m_pGrid->GetCell(playerPos);

	m_pGrid->PlaceObject(new BombPrefab(m_PlayerStats.blastRadius, m_pGrid), playerCell);

}

void BombermanCharacter::HandleInputAndMovement(const SceneContext& sceneContext)
{
	const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };

	//************
	//HANDLE INPUT

	//## Input Gathering (move)
	XMFLOAT2 move{};
	move.y = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward) ? 1.0f :
		(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward) ? -1.0f : 0.0f);

	move.x = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight) ? 1.0f :
		(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft) ? -1.0f : 0.0f);

	if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_PlaceBomb))
		SpawnBomb();


	//Change the look direction based on the input (in degrees)
	if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		m_YRotation = 180.f;
	else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		m_YRotation = 0.f;
	if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		m_YRotation = -90.f;
	else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		m_YRotation = 90.f;


	//************************
	//GATHERING TRANSFORM INFO

	constexpr XMFLOAT3 forwardFloat{ 0, 0,1 };
	constexpr XMFLOAT3 rightFloat{ 1, 0,0 };
	const XMVECTOR forward{ XMLoadFloat3(&forwardFloat) };
	const XMVECTOR right{ XMLoadFloat3(&rightFloat) };


	//********
	//MOVEMENT

	//## Horizontal Velocity (Forward/Backward/Right/Left)
	const float currentMoveAcceleration{ m_MoveAcceleration * elapsedSec };
	//If the character is moving (= input is pressed)
	if (abs(move.x) > 0.0f || abs(move.y) > 0.0f)
	{
		//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
		const XMVECTOR moveDirection{ forward * move.y + right * move.x };
		XMStoreFloat3(&m_CurrentDirection, moveDirection);

		m_MoveSpeed += currentMoveAcceleration;
		m_MoveSpeed = std::min(m_MoveSpeed, m_CharacterDesc.maxMoveSpeed);
	}
	else
	{
		m_MoveSpeed -= currentMoveAcceleration;
		m_MoveSpeed = std::max(m_MoveSpeed, 0.0f);
	}

	m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
	m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

	if (!(m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_TotalVelocity.y -= m_FallAcceleration * elapsedSec;
		m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_CharacterDesc.maxFallSpeed);
	}
	else
		m_TotalVelocity.y = -0.1f;

	//************
	//DISPLACEMENT

	const XMFLOAT3 displacement
	{
		m_TotalVelocity.x * elapsedSec,
		m_TotalVelocity.y * elapsedSec,
		m_TotalVelocity.z * elapsedSec
	};
	m_pControllerComponent->Move(displacement);
}


void BombermanCharacter::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");
	}
}