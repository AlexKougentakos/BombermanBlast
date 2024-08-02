#include "stdafx.h"
#include "BombermanCharacter.h"

#include "SpherePrefab.h"
#include "Scenes/GameScene/BombermanBlastScene.h"
#include "Components/Grid.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Prefabs/BombPrefab.h"
#include <Components/GameObjectManager.h>

int BombermanCharacter::m_InstanceCounter = 0;

BombermanCharacter::BombermanCharacter(const CharacterDesc& characterDesc, GridComponent* const pGrid) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_pGrid(pGrid)
{
	++m_InstanceCounter;
	m_PlayerIndex = m_InstanceCounter;
	SetTag(L"Player");
}

void BombermanCharacter::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	m_pControllerComponent->SetCollisionIgnoreGroup(CollisionGroup::Group5);

	m_pControllerComponent->SetContactOffset(0.0001f);
	m_pControllerComponent->SetStepOffset(0);

	const auto pModelForCharacter = new GameObject();
	AddChild(pModelForCharacter);
	pModelForCharacter->GetTransform()->Translate(0, -20.f, 0);
	//Add Model to Character
	m_pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pFaceMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pFaceMaterial->SetDiffuseTexture(L"Textures/bom_face01.png");

	m_pModelComponent = pModelForCharacter->AddComponent(new ModelComponent(L"Meshes/CharacterFixed.ovm", false));

	switch (GetPlayerColour())
	{
	case PlayerColour::RED:
		m_pMaterial->SetDiffuseTexture(L"Textures/bomberman00.png");
		break;
	case PlayerColour::YELLOW:
		m_pMaterial->SetDiffuseTexture(L"Textures/bomberman00.png");
		break;
	case PlayerColour::BLUE:
		m_pMaterial->SetDiffuseTexture(L"Textures/bomberman00.png");
		break;
	case PlayerColour::WHITE:
		m_pMaterial->SetDiffuseTexture(L"Textures/bomberman00.png");
		break;
	}

	m_pModelComponent->SetMaterial(m_pMaterial);
	m_pModelComponent->SetMaterial(pFaceMaterial, 0);
	

	m_pModelAnimator = m_pModelComponent->GetAnimator();
	m_pModelAnimator->SetAnimation(m_AnimationClipId);
	m_pModelAnimator->Play();

	//Add the shadow blob to the player
	const auto pShadowBlobObject(new GameObject());
	AddChild(pShadowBlobObject);
	const auto modelComp = pShadowBlobObject->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));

	auto mat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	modelComp->SetMaterial(mat);

	modelComp->GetTransform()->Scale(0.3f);
	modelComp->GetTransform()->Translate(0.f, 0.f, 20.f);
}

void BombermanCharacter::Reset()
{
	m_IsDead = false;
	m_PlayerState = PlayerState::Idle;
	m_PlayerStats = PlayerStats{};
	m_RemainingBombs = m_PlayerStats.maxBombs;
}


void BombermanCharacter::Update(const SceneContext& sceneContext)
{
	if (!sceneContext.settings.inDebug)
	{
		HandleInputAndMovement(sceneContext);

		switch (m_PlayerState)
		{
			case PlayerState::Idle:
				ChangeAnimationClip(1);
			m_pModelAnimator->SetAnimationSpeed(1.5f);
				break;
			case PlayerState::Moving:
				ChangeAnimationClip(2);
			m_pModelAnimator->SetAnimationSpeed(3.5f);
				break;
			case PlayerState::Dead:
				ChangeAnimationClip(5);
				m_ElapsedDeathTimer += sceneContext.pGameTime->GetElapsed();

			//Player flickering
				const float flickerDuration = 1.0f / m_DeathFlickerSpeed;
				const float flickerProgress = fmod(m_ElapsedDeathTimer, flickerDuration) / flickerDuration;
				[[maybe_unused]]const float flickerOpacity = (flickerProgress < 0.5f) ? 0.1f : 1.0f;

				//m_pMaterial->SetOpacity(flickerOpacity);

				if (m_ElapsedDeathTimer >= m_DeathAnimTime)
				{
					notifyObservers("Player Death");
					m_IsDead = true;
					m_pGrid->GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(this);
					//m_pGrid->DeleteSpecificObject(this);
				}
				break;
		}
	}
}

void BombermanCharacter::SpawnBomb()
{
	if (m_RemainingBombs <= 0) return;

 	const XMFLOAT3 playerPos = GetTransform()->GetPosition();
	GridCell& playerCell = m_pGrid->GetCell(playerPos);

	const auto gameObjectManager = m_pGrid->GetGameObject()->GetComponent<GameObjectManager>();

	m_pGrid->PlaceObject(gameObjectManager->CreateGameObject<BombPrefab>(m_PlayerStats.blastRadius, m_pGrid, this), playerCell);
	m_RemainingBombs--;
}

void BombermanCharacter::GiveBackBomb()
{
	//This will likely never be false, but just in case
	if (m_RemainingBombs +1 <= m_PlayerStats.maxBombs)
		++m_RemainingBombs;
}

void BombermanCharacter::ChangeAnimationClip(UINT animationID)
{
	if (m_AnimationClipId != animationID)
	{
		m_AnimationClipId = animationID;
		m_pModelAnimator->SetAnimation(m_AnimationClipId);
		m_pModelAnimator->Play();
	}
}

void BombermanCharacter::HandleInputAndMovement(const SceneContext& sceneContext)
{
    if (m_PlayerState == PlayerState::Dead)
        return;

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

    if (InputManager::IsGamepadConnected(GamepadIndex(m_PlayerIndex - 1))) //-1 because they start at 0
    {
        if (move.x != 0 || move.y != 0) return;

        move = InputManager::GetThumbstickPosition(true, GamepadIndex(m_PlayerIndex - 1));
    }

	//************************
	//MODEL ROTATION
    if (move.x != 0 || move.y != 0)
    {
        // Calculate the target rotation
        const float targetRotation = -std::atan2(move.x, -move.y) * 180.f / float(3.1415);

        // Define the rotation speed (adjust this value to change how quickly the player turns)
        constexpr float rotationSpeed = 720.0f; // degrees per second

        // Calculate the shortest angle between current and target rotation
        float angleDiff = targetRotation - m_YRotation;
        if (angleDiff > 180.0f) angleDiff -= 360.0f;
        else if (angleDiff < -180.0f) angleDiff += 360.0f;

        // Calculate the maximum rotation for this frame
        const float maxRotation = rotationSpeed * elapsedSec;

        // Clamp the rotation to the maximum allowed for this frame
        const float actualRotation = std::clamp(angleDiff, -maxRotation, maxRotation);

        // Update rotation
        m_YRotation += actualRotation;
        if (m_YRotation > 180.0f) m_YRotation -= 360.0f;
        else if (m_YRotation < -180.0f) m_YRotation += 360.0f;

        // Apply the rotation to the model
        m_pModelComponent->GetTransform()->Rotate(0, m_YRotation, 0);

        m_PlayerState = PlayerState::Moving;
    }
    else m_PlayerState = PlayerState::Idle;

    //************************
    //GATHERING TRANSFORM INFO

    constexpr XMFLOAT3 forwardFloat{ 0, 0,1 };
    constexpr XMFLOAT3 rightFloat{ 1, 0,0 };
    const XMVECTOR forward{ XMLoadFloat3(&forwardFloat) };
    const XMVECTOR right{ XMLoadFloat3(&rightFloat) };

    //********
    //MOVEMENT

    //Each powerup should result in a % speed increase
    constexpr float speedPercentageIncrease{20.f};
    constexpr  float toDivideWith{ 100.f / speedPercentageIncrease };
    const float speedMultiplierFromPowerUp{ 1 + m_PlayerStats.speed / toDivideWith };

    //## Horizontal Velocity (Forward/Backward/Right/Left)
    const float currentMoveAcceleration{ m_MoveAcceleration * elapsedSec * speedMultiplierFromPowerUp };
    //If the character is moving (= input is pressed)
    if (abs(move.x) > 0.0f || abs(move.y) > 0.0f)
    {
        //Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
        const XMVECTOR moveDirection{ forward * move.y + right * move.x };
        XMStoreFloat3(&m_CurrentDirection, moveDirection);
        XMVector3Normalize(moveDirection);

        m_MoveSpeed += currentMoveAcceleration;
        m_MoveSpeed = std::min(m_MoveSpeed, m_CharacterDesc.maxMoveSpeed * speedMultiplierFromPowerUp);
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

void BombermanCharacter::Kill()
{
	m_PlayerState = PlayerState::Dead;
}

void BombermanCharacter::AddPoint()
{
	++m_PlayerScore;
	notifyObservers("Score Increase");
}

void BombermanCharacter::DrawImGui()
{
	if (ImGui::CollapsingHeader(std::format("Player [{:1}]", m_PlayerIndex).c_str()))
	{
		if (ImGui::TreeNode("Default Info"))
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
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Character Stats"))
		{
			constexpr float speedPercentageIncrease{ 20.f };
			constexpr  float toDivideWith{ 100.f / speedPercentageIncrease };
			const float speedMultiplierFromPowerUp{ 100 * m_PlayerStats.speed / toDivideWith };

			ImGui::Text(std::format("Bombs: {} ({:1})", m_PlayerStats.maxBombs, m_RemainingBombs).c_str());
			ImGui::Text(std::format("Blast Radius: {}", m_PlayerStats.blastRadius).c_str());
			ImGui::Text(std::format("Speed: {} (+{:.1f}% %)", m_PlayerStats.speed, speedMultiplierFromPowerUp).c_str());

			ImGui::Dummy({ 0.f,5.f });
			if (ImGui::DragInt("Bombs", &m_PlayerStats.maxBombs, 0.1f, 0, 0) ||
				ImGui::DragInt("Blast Radius", &m_PlayerStats.blastRadius, 0.1f, 0, 0) ||
				ImGui::DragInt("Speed", &m_PlayerStats.speed, 0.1f, 0, 0))
			{
				// If you want to handle some updates when these values change, do it here
			}

			ImGui::TreePop();
		}
	}
}