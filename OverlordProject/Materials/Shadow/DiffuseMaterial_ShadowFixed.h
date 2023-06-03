#pragma once

class DiffuseMaterial_ShadowFixed final : public Material<DiffuseMaterial_ShadowFixed>
{
public:
	DiffuseMaterial_ShadowFixed(const XMFLOAT3& lightDir);
	~DiffuseMaterial_ShadowFixed() override = default;

	DiffuseMaterial_ShadowFixed(const DiffuseMaterial_ShadowFixed& other) = delete;
	DiffuseMaterial_ShadowFixed(DiffuseMaterial_ShadowFixed&& other) noexcept = delete;
	DiffuseMaterial_ShadowFixed& operator=(const DiffuseMaterial_ShadowFixed& other) = delete;
	DiffuseMaterial_ShadowFixed& operator=(DiffuseMaterial_ShadowFixed&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pDiffuseTexture{};
	XMFLOAT3 m_LightDirection{};
};

