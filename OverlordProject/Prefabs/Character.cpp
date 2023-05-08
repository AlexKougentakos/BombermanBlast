#include "stdafx.h"
#include "Character.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height * .5f, 0.f);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
		const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{};
		move.y = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward) ? 1.0f :
		(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward) ? -1.0f : 0.0f);

		move.x = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight) ? 1.0f :
		(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft) ? -1.0f : 0.0f);

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f };
		constexpr int mouseButton{ 1 };
		if (InputManager::IsMouseButton(InputState::down, mouseButton))
		{
			const auto& mouseMovement{ InputManager::GetMouseMovement() };
			look.x = static_cast<float>(mouseMovement.x);
			look.y = static_cast<float>(mouseMovement.y);
		}

		//************************
		//GATHERING TRANSFORM INFO

		const TransformComponent* pTransform{ GetTransform() };
		const XMVECTOR forward{ XMLoadFloat3(&pTransform->GetForward()) };
		const XMVECTOR right{ XMLoadFloat3(&pTransform->GetRight()) };

		//***************
		//CAMERA ROTATION

		const float rotationAmount{ elapsedSec * m_CharacterDesc.rotationSpeed };
		m_TotalYaw += look.x * rotationAmount;
		m_TotalPitch += look.y * rotationAmount;
		
		GetTransform()->Rotate(0.0f, m_TotalYaw, 0.0f);
		sceneContext.pCamera->GetTransform()->Rotate(m_TotalPitch, 0.0f, 0.0f);

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
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
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
}

void Character::DrawImGui()
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

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}