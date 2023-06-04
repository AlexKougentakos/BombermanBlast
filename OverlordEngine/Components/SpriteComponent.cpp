#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color) :
    m_SpriteAsset(spriteAsset),
    m_Pivot(pivot),
    m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/ )
{
    m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
    m_SpriteAsset = spriteAsset;
    m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& /*sceneContext*/ )
{
    if (!m_pTexture)
        return;

    //// Here you need to draw the SpriteComponent using the Draw of the sprite renderer
    //// The sprite renderer is a singleton
    SpriteRenderer* spriteRenderer{ SpriteRenderer::Get() };

    //// you will need to position, the rotation and the scale
    TransformComponent* transformPRS = m_pGameObject->GetTransform();

    ////Transform Rotation to Euler
    const XMFLOAT3 rotation{ MathHelper::QuaternionToEuler(transformPRS->GetRotation()) };

    const XMFLOAT2 position{ transformPRS->GetPosition().x, transformPRS->GetPosition().y };

    const float zPosition{ transformPRS->GetPosition().z };


    spriteRenderer->AppendSprite(m_pTexture, position, m_Color, m_Pivot, XMFLOAT2{ transformPRS->GetScale().x, transformPRS->GetScale().y }, rotation.z, zPosition);


    //Here you need to draw the SpriteComponent using the Draw of the sprite renderer
    // The sprite renderer is a singleton
    // you will need to position (X&Y should be in screenspace, Z contains the depth between [0,1]), the rotation and the scale from the owning GameObject
    // You can use the MathHelper::QuaternionToEuler function to help you with the z rotation 
}