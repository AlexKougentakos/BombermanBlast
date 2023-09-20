#pragma once

class BombermanCharacter;

class RoundWinners final : public GameObject
{
public:
	RoundWinners() = default;
	virtual ~RoundWinners() override = default;

	virtual void Initialize(const SceneContext&) override;
	virtual void Update(const SceneContext&) override;

	RoundWinners(const RoundWinners& other) = delete;
	RoundWinners(RoundWinners&& other) noexcept = delete;
	RoundWinners& operator=(const RoundWinners& other) = delete;
	RoundWinners& operator=(RoundWinners&& other) noexcept = delete;

	
	SpriteComponent* m_pRoundOverSprite{};
};
