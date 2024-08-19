#pragma once
class SoundManager final: public Singleton<SoundManager>
{
public:
	SoundManager(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) noexcept = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& other) noexcept = delete;

	FMOD::System* GetSystem() const { return m_pFmodSystem; }

	void Play(const std::wstring& soundPath, float volume = 1.f, bool isLooping = false) const;
	void PlayMusic(const std::wstring& musicPath, float volume = 1.f, bool overrideTrack = false);
	void StopMusic();

protected:
	void Initialize() override;

private:
	friend class Singleton<SoundManager>;
	SoundManager();
	~SoundManager();

	FMOD::System* m_pFmodSystem = nullptr;

	FMOD::Sound* m_pCurrentMusic{};
	FMOD::Channel* m_pMusicChannel{};
	std::wstring m_CurrentMusicPath{};
};

