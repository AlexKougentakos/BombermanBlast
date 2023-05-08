#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	if (m_IsPlaying && m_ClipSet)
	{

		auto totalTicks{sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed};
		if (totalTicks > m_CurrentClip.duration) totalTicks = fmodf(totalTicks, m_CurrentClip.duration);

		if (m_Reversed)
		{
			m_TickCount -= totalTicks;
			if (m_TickCount < 0) m_TickCount += m_CurrentClip.duration;
		}
		else
		{
			m_TickCount += totalTicks;
			if (m_TickCount > m_CurrentClip.duration) m_TickCount -= m_CurrentClip.duration;
		}

		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:

		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount

		for (int i{}; i < m_CurrentClip.keys.size(); ++i)
		{
			const auto& key{ m_CurrentClip.keys[i] };

			if (key.tick > m_TickCount)
			{
				keyB = key;
				keyA = m_CurrentClip.keys[i - 1];
				break;
			}
		}

		const float blendFactor{ 1 - (keyB.tick - m_TickCount) / (m_CurrentClip.duration / (m_CurrentClip.keys.size() - 1)) };
		for (int i{}; i < m_pMeshFilter->GetBoneCount(); ++i)
		{
			const auto& transformA{ keyA.boneTransforms[i] };
			const auto& transformB{ keyB.boneTransforms[i] };

			XMVECTOR scaleA{};
			XMVECTOR rotationA{};
			XMVECTOR translationA{};
			XMMatrixDecompose(&scaleA, &rotationA, &translationA, XMLoadFloat4x4(&transformA));

			XMVECTOR scaleB{};
			XMVECTOR rotationB{};
			XMVECTOR translationB{};
			XMMatrixDecompose(&scaleB, &rotationB, &translationB, XMLoadFloat4x4(&transformB));

			const XMVECTOR scaleLerp{ XMVectorLerp(scaleA, scaleB, blendFactor) };
			const XMVECTOR rotationLerp{ XMQuaternionSlerp(rotationA, rotationB, blendFactor) };
			const XMVECTOR translationLerp{ XMVectorLerp(translationA, translationB, blendFactor) };

			const XMMATRIX transform{ XMMatrixScalingFromVector(scaleLerp) * 
				XMMatrixRotationQuaternion(rotationLerp) * 
				XMMatrixTranslationFromVector(translationLerp) };

			XMStoreFloat4x4(&m_Transforms[i], transform);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	m_ClipSet = false;

	for (auto& animationClip : m_pMeshFilter->GetAnimationClips())
	{
		if (animationClip.name == clipName)
		{
			SetAnimation(animationClip);
			return;
		}
	}

	Reset();
	Logger::LogWarning(L"No animation found with name " + clipName);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;

	const auto animations{ m_pMeshFilter->GetAnimationClips() };

	if (clipNumber < animations.size())
	{
		SetAnimation(animations[clipNumber]);
		return;
	}

	Reset();
	Logger::LogWarning(L"No animation with index " + std::to_wstring(clipNumber) + L" was found");
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;

	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause) m_IsPlaying = false;
	m_TickCount = 0;
	m_AnimationSpeed = 1.f;

	if (m_ClipSet)
	{
		m_Transforms = m_CurrentClip.keys[0].boneTransforms;
		return;
	}

	constexpr XMFLOAT4X4 identityMatrix
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	m_Transforms = std::vector<XMFLOAT4X4>(m_pMeshFilter->GetBoneCount(), identityMatrix);
}
