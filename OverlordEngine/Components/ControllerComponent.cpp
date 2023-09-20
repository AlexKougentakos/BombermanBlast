#include "stdafx.h"
#include "ControllerComponent.h"

ControllerComponent::ControllerComponent(const PxCapsuleControllerDesc& controllerDesc) :
	m_ControllerDesc{ controllerDesc }
{
	
}

ControllerComponent::~ControllerComponent()
{
	m_pController->release();
}

void ControllerComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	if (!m_IsInitialized)
	{
		const XMFLOAT3& position{ GetTransform()->GetPosition() };
		m_ControllerDesc.position = PxExtendedVec3{ position.x, position.y, position.z };
		m_ControllerDesc.userData = this;

		const auto& pControllerManager{ GetGameObject()->GetScene()->GetPhysxProxy()->GetControllerManager() };

		m_pController = pControllerManager->createController(m_ControllerDesc);
		ASSERT_NULL_(m_pController);

		m_pController->getActor()->userData = this;

		//SetCollisionGroup(static_cast<CollisionGroup>(m_CollisionGroups.word0));
		//SetCollisionIgnoreGroup(static_cast<CollisionGroup>(m_CollisionGroups.word1));
	}
}

void ControllerComponent::OnSceneDetach(GameScene*)
{
	if (m_pController)
	{
		m_pController->getActor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	}
}

void ControllerComponent::OnSceneAttach(GameScene*)
{
	if (m_pController)
	{
		m_pController->getActor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	}
}

void ControllerComponent::OnOwnerAttach(GameObject* pOwner)
{
	pOwner->GetTransform()->SetControllerComponent(this);
}

void ControllerComponent::OnOwnerDetach(GameObject* pOwner)
{
	pOwner->GetTransform()->SetControllerComponent(nullptr);
}

void ControllerComponent::ApplyFilterData() const
{
	if (m_pController != nullptr)
	{
		const auto actor = m_pController->getActor();
		const auto numShapes = actor->getNbShapes();
		const auto shapes = new PxShape * [numShapes];

		const auto numPointers = actor->getShapes(shapes, numShapes);
		for (PxU32 i = 0; i < numPointers; i++)
		{
#pragma warning (push)
#pragma warning (disable: 6385)
			const auto shape = shapes[i];
#pragma warning (pop)
			shape->setSimulationFilterData(m_CollisionGroups);
			shape->setQueryFilterData(m_CollisionGroups);
		}
		delete[] shapes;
	}
}

CollisionGroup ControllerComponent::GetCollisionGroup() const
{
	return static_cast<CollisionGroup>(m_CollisionGroups.word0);
}

void ControllerComponent::Translate(const XMFLOAT3& pos) const
{
	ASSERT_NULL_(m_pController);
	m_pController->setPosition(PhysxHelper::ToPxExtendedVec3(pos));
}

class MyControllerFilterCallback : public PxControllerFilterCallback
{
public:
    bool filter(const PxController&, const PxController&) override
    {
        // Return true to ignore the collision between controller 'a' and 'b'
        return false;
    }
};

class MyControllerFilterCallback2 : public PxQueryFilterCallback
{
public:
	virtual PxQueryHitType::Enum preFilter(
		const PxFilterData& /*filterData*/,
		const PxShape* shape,
		const PxRigidActor* /*actor*/,
		PxHitFlags& /*queryFlags*/) override
	{
	
		if (shape != nullptr && shape->getActor() != nullptr && shape->getActor()->getName() == std::string("level"))
			__debugbreak();

		return PxQueryHitType::eBLOCK;
	}

	virtual PxQueryHitType::Enum postFilter(
		const PxFilterData& filterData,
		const PxQueryHit& /*hit*/) override
	{
		// Modify post-filter behavior if needed

		if (filterData.word0 == static_cast<UINT32>(CollisionGroup::Group2))
		{
			__debugbreak();
		}
		return PxQueryHitType::eNONE;
	}

private:
	PxFilterData m_CollisionGroups;
};

void ControllerComponent::Move(const XMFLOAT3& displacement, float minDistance)
{
	ASSERT_NULL_(m_pController);

	PxFilterData filterData;
	filterData.word0 = PxU32(CollisionGroup::Level | CollisionGroup::Bomb_Outside | CollisionGroup::Brick);

	MyControllerFilterCallback filterCallback;
	//MyControllerFilterCallback2 filterCallback2;

	PxControllerFilters filters;
	filters.mCCTFilterCallback = &filterCallback;
	filters.mFilterData = &filterData;
	//filters.mFilterCallback = &filterCallback2;
	

	m_CollisionFlag = m_pController->move(PhysxHelper::ToPxVec3(displacement), minDistance, 0, filters, nullptr);
}

XMFLOAT3 ControllerComponent::GetPosition() const
{
	ASSERT_NULL_(m_pController);
	return PhysxHelper::ToXMFLOAT3(m_pController->getPosition());
}

XMFLOAT3 ControllerComponent::GetFootPosition() const
{
	ASSERT_NULL_(m_pController);
	return PhysxHelper::ToXMFLOAT3(m_pController->getFootPosition());
}

void ControllerComponent::SetFootPosition(const XMFLOAT3 footPosition) const
{
	ASSERT_NULL_(m_pController);
	m_pController->setFootPosition(PhysxHelper::ToPxExtendedVec3(footPosition));
}

PxF32 ControllerComponent::GetContactOffset() const
{
	ASSERT_NULL_(m_pController);
	return m_pController->getContactOffset();
}

void ControllerComponent::SetContactOffset(PxF32 offset) const
{
	ASSERT_NULL_(m_pController);
	m_pController->setContactOffset(offset);
}

void ControllerComponent::SetCollisionGroup(CollisionGroup groups)
{
	m_CollisionGroups.word0 = PxU32(groups);
	ApplyFilterData();
}

void ControllerComponent::SetCollisionIgnoreGroup(CollisionGroup ignoreGroups)
{
	m_CollisionGroups.word1 = PxU32(ignoreGroups);
	ApplyFilterData();
}

PxF32 ControllerComponent::GetStepOffset() const
{
	ASSERT_NULL_(m_pController);
	return m_pController->getStepOffset();
}

void ControllerComponent::SetStepOffset(const float offset) const
{
	ASSERT_NULL_(m_pController);
	m_pController->setStepOffset(offset);
}

