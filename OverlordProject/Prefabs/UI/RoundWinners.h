#pragma once

enum class PlayerColour;
class BombermanCharacter;

class RoundWinners final : public GameObject
{
public:
	RoundWinners(const std::vector<int>& playerIndices);
	virtual ~RoundWinners() override = default;

	virtual void Initialize(const SceneContext&) override;
	virtual void Update(const SceneContext&) override;

	RoundWinners(const RoundWinners& other) = delete;
	RoundWinners(RoundWinners&& other) noexcept = delete;
	RoundWinners& operator=(const RoundWinners& other) = delete;
	RoundWinners& operator=(RoundWinners&& other) noexcept = delete;

private:	
	SpriteComponent* m_pRoundWinnersSprite{};
	SpriteComponent* m_pPlayerHead{};
	std::map<PlayerColour, std::wstring> m_PlayerColourToSpritePathMap{};
	
	std::vector<int> m_PlayerIndices{};
};
