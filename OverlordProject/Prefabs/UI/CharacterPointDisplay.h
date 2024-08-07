#pragma once

class GameLoopManager;
class BombermanCharacter;
enum class PlayerColour;

class CharacterPointDisplay : public GameObject, public Observer<GameLoopManager>
{
public:
	CharacterPointDisplay(const int characterIndex, GameLoopManager* pGameLoopManager, const XMFLOAT2& position);
	virtual ~CharacterPointDisplay() override = default;

	CharacterPointDisplay(const CharacterPointDisplay& other) = delete;
	CharacterPointDisplay(CharacterPointDisplay&& other) noexcept = delete;
	CharacterPointDisplay& operator=(const CharacterPointDisplay& other) = delete;
	CharacterPointDisplay& operator=(CharacterPointDisplay&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& sceneContext) override;

protected:
	void Draw(const SceneContext&) override;
	virtual void OnNotify(GameLoopManager* source, const std::string& field) override;
private:

	std::map<PlayerColour, std::wstring> m_PlayerColourToSpritePathMap{};

	SpriteComponent* m_pSpriteComponent{};
	GameLoopManager* m_pGameLoopManager{};
	PlayerColour m_PlayerColour{};

	XMFLOAT2 m_SpritePosition{};
	XMFLOAT2 m_TextPosition{};

	int m_ScoreToDisplay{};
	int m_CharacterIndex{};

	SpriteFont* m_pFont{};
};
