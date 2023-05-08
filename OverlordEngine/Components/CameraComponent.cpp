#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	const auto mousePos = InputManager::GetMousePosition();

	const auto pScene{ GetGameObject()->GetScene() };

	const float halfWidth{ pScene->GetSceneContext().windowWidth / 2.f };
	const float halfHeight{ GetGameObject()->GetScene()->GetSceneContext().windowHeight / 2.f };

	const XMFLOAT2 PositionNDC{ (mousePos.x - halfWidth) / halfWidth,
		(halfHeight - mousePos.y) / halfHeight };

	const XMVECTOR nearPoint{ XMVector3TransformCoord(XMVECTOR{ PositionNDC.x, PositionNDC.y, 0.f, 0.f }
	, XMLoadFloat4x4(&GetViewProjectionInverse()))};

	const XMVECTOR farPoint{ XMVector3TransformCoord(XMVECTOR{ PositionNDC.x, PositionNDC.y, 1.f, 0.f }
	, XMLoadFloat4x4(&GetViewProjectionInverse())) };

	PxQueryFilterData filterData{};

	filterData.data.word0 = ~UINT(ignoreGroups);

	// We have to convert the near & far point to PxVec3 objects
	// We will store it in a float3 so that we can easily access the data
	XMFLOAT3 rayStart{};
	XMStoreFloat3(&rayStart, nearPoint);
	XMFLOAT3 rayEnd{};
	XMStoreFloat3(&rayEnd, farPoint);

	// Then simply re-assign the values.
	const PxVec3 rayOrigin{ rayStart.x, rayStart.y, rayStart.z };
	const PxVec3 rayDirection{ PxVec3{rayEnd.x - rayOrigin.x, rayEnd.y - rayOrigin.y, rayEnd.z - rayOrigin.z}.getNormalized() };

	PxRaycastBuffer hit{};
	if (pScene->GetPhysxProxy()->Raycast(rayOrigin, rayDirection, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		const BaseComponent* pComponent{ static_cast<BaseComponent*>(hit.block.actor->userData) };
		return pComponent->GetGameObject();
	}

	return nullptr;
}