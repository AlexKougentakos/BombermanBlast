#pragma once

class BombermanCharacter;
enum class PlayerColour;

class CharacterPointDisplay : public GameObject, public Observer<BombermanCharacter>
{
public:
	CharacterPointDisplay(BombermanCharacter* bombermanCharacter, const XMFLOAT2& position);
	virtual ~CharacterPointDisplay() override = default;

	CharacterPointDisplay(const CharacterPointDisplay& other) = delete;
	CharacterPointDisplay(CharacterPointDisplay&& other) noexcept = delete;
	CharacterPointDisplay& operator=(const CharacterPointDisplay& other) = delete;
	CharacterPointDisplay& operator=(CharacterPointDisplay&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;

protected:
	virtual void Update(const SceneContext& sceneContext) override;
	virtual void OnNotify(BombermanCharacter* source, const std::string& field) override;
private:


	std::map<PlayerColour, std::wstring> m_PlayerColourToSpritePathMap{};

	SpriteComponent* m_pSpriteComponent{};
	BombermanCharacter* m_pBombermanCharacter{};
	PlayerColour m_PlayerColour{};

	XMFLOAT2 m_SpritePosition{};
	XMFLOAT2 m_TextPosition{};

	int m_ScoreToDisplay{};

	SpriteFont* m_pFont{};
};
