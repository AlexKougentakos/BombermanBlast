#pragma once

class BoneObject;

class SoftwareSkinningScene_2 final : public GameScene
{
public:
	SoftwareSkinningScene_2() : GameScene(L"Software Skinning Scene 2") {};
	~SoftwareSkinningScene_2() override = default;

	SoftwareSkinningScene_2(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2(SoftwareSkinningScene_2&& other) noexcept = delete;
	SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& other) = delete;
	SoftwareSkinningScene_2& operator=(SoftwareSkinningScene_2&& other) noexcept = delete;

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
		VertexSoftwareSkinned(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT4 color)
			: transformedVertex{ position, normal, color }
			, originalVertex{ position, normal, color } {}

		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};
	};

	void InitializeVertices(float length);
	void UpdateMeshDrawer() const;

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};
};
