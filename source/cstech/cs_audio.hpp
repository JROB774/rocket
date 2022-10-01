#pragma once

CS_PUBLIC_SCOPE::sfx
{
    typedef u32 SoundRef;

    DECLARE_PRIVATE_STRUCT(Sound);
    DECLARE_PRIVATE_STRUCT(Music);

    static const SoundRef k_invalidSoundRef = 0xFFFFFFFF;

    CS_API void InitAudio();
    CS_API void QuitAudio();

    CS_API void SetSoundVolume(f32 volume); // [0-1]
    CS_API void SetMusicVolume(f32 volume); // [0-1]

    CS_API f32 GetSoundVolume();
    CS_API f32 GetMusicVolume();

    CS_API bool IsSoundOn();
    CS_API bool IsMusicOn();

    CS_API bool IsMusicPlaying();

    // Sound
    CS_API bool LoadSound(Sound& sound, std::string fileNmae);
    CS_API void FreeSound(Sound& sound);
    CS_API SoundRef PlaySound(std::string soundName, s32 loops = 0);
    CS_API SoundRef PlaySound(Sound sound, s32 loops = 0);
    CS_API void StopSound(SoundRef soundRef);

    // Music
    CS_API bool LoadMusic(Music& music, std::string fileName);
    CS_API void FreeMusic(Music& music);
    CS_API void PlayMusic(std::string musicName, s32 loops = 0);
    CS_API void PlayMusic(Music music, s32 loops = 0);
    CS_API void ResumeMusic();
    CS_API void PauseMusic();
    CS_API void StopMusic();
}

DECLARE_ASSET(cs::sfx::Sound)
{
public:
    cs::sfx::Sound m_data;

    bool        Load(std::string fileName) override;
    void        Free() override;
    const char* GetPath() const override;
    const char* GetExt() const override;
    const char* GetType() const override;
};

DECLARE_ASSET(cs::sfx::Music)
{
public:
    cs::sfx::Music m_data;

    bool        Load(std::string fileName) override;
    void        Free() override;
    const char* GetPath() const override;
    const char* GetExt() const override;
    const char* GetType() const override;
};
