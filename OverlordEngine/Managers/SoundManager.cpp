#include "stdafx.h"
#include "SoundManager.h"

#include <thread>

SoundManager::SoundManager()
{
	Initialize();
}

SoundManager::~SoundManager()
{
	if (m_pFmodSystem)
	{
#pragma warning(push)
#pragma warning(disable : 26812)
		m_pFmodSystem->release();
#pragma warning(pop)
	}
}

void SoundManager::Initialize()
{
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_ERROR);

	unsigned int version{};
	int numdrivers{};

	/*
	Create a System object and initialize.
	*/
	HANDLE_ERROR(System_Create(&m_pFmodSystem));
	HANDLE_ERROR(m_pFmodSystem->getVersion(&version));
	ASSERT_IF(version < FMOD_VERSION, L"Initialization Failed!\n\nYou are using an old version of FMOD {:#x}. This program requires {:#x}\n",
		version, FMOD_VERSION)

	HANDLE_ERROR(m_pFmodSystem->getNumDrivers(&numdrivers));

	if (numdrivers == 0)
	{
		HANDLE_ERROR(m_pFmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
	else
	{
		HANDLE_ERROR(m_pFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr));
	}
}

void SoundManager::Play(const std::wstring& soundPath, float volume, bool isLooping) const
{
    std::jthread([this, soundPath, volume, isLooping]()
        {
            const std::wstring fullPath = m_GameContext.contentRoot + L"Sounds/" + soundPath;
    // Convert wide string to multi-byte string
    const int narrowStrSize = WideCharToMultiByte(
        CP_UTF8,        // UTF-8 encoding
        0,              // Default flags
        fullPath.c_str(),
        -1,             // null-terminated input string
        nullptr,        // output buffer is null, so return required size
        0,
        nullptr,
        nullptr
    );

    std::string narrowStr(narrowStrSize, '\0');

    WideCharToMultiByte(
        CP_UTF8,        // UTF-8 encoding
        0,              // Default flags
        fullPath.c_str(),
        -1,             // null-terminated input string
        &narrowStr[0],  // output buffer
        narrowStrSize,
        nullptr,
        nullptr
    );

    FMOD::Sound* sound = NULL;
    FMOD::Channel* channel = NULL;
    FMOD_RESULT result{};

    result = m_pFmodSystem->createSound(narrowStr.c_str(), FMOD_DEFAULT, 0, &sound);
    if (result != FMOD_OK)
    {
        std::wcerr << "Cannot play sound: " << fullPath << ". Error: " << result << std::endl;
        return;
    }
    sound->setMode(isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    result = m_pFmodSystem->playSound(sound, 0, false, &channel);
    if (result != FMOD_OK)
    {
        std::wcerr << "Cannot play sound: " << fullPath << ". Error: " << result << std::endl;
        return;
    }

    channel->setVolume(volume);

    bool isPlaying = true;
    while (isPlaying)
    {
        m_pFmodSystem->update();
        channel->isPlaying(&isPlaying);
    }

    // Sound is no longer playing, clean up
    result = sound->release();
        }).detach();
}
