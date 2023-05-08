#pragma once

class BoneObject;

class SoftwareSkinningScene_3 final : public GameScene
{
public:
	SoftwareSkinningScene_3() : GameScene(L"Software Skinning Scene 3") {};
	~SoftwareSkinningScene_3() override = default;

	SoftwareSkinningScene_3(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3(SoftwareSkinningScene_3&& other) noexcept = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3& operator=(SoftwareSkinningScene_3&& other) noexcept = delete;

protected:
	void Initialize() override;
	void UpdateBones();
	void Update() override;
	void OnGUI() override;

private:
	BoneObject* m_pBone1{}, * m_pBone2{};

	float m_BoneRotation{};
	int m_RotationSign{ 1 };

	bool m_AutoRotate{ true };
	XMFLOAT3 m_Bone1Rot{};
	XMFLOAT3 m_Bone2Rot{};

	struct VertexSoftwareSkinned
	{
		VertexSoftwareSkinned(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT4 color, float bone0weight)
			: transformedVertex{ position, normal, color }
			, originalVertex{ position, normal, color }
			, blendWeight{ bone0weight } {}

		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};
		float blendWeight{};
	};

	void InitializeVertices(float length);
	void UpdateMeshDrawer() const;

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};
};
