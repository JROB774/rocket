#pragma once

#include "cs_define.hpp"
#include "cs_assets.hpp"

CS_PUBLIC_SCOPE::sfx
{
    typedef u32 SoundRef;

    CS_DECLARE_PRIVATE_STRUCT(Sound);
    CS_DECLARE_PRIVATE_STRUCT(Music);

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

CS_PUBLIC_SCOPE
{
    CS_DECLARE_ASSET(sfx::Sound)
    {
    public:
        sfx::Sound m_data;

        bool        Load(std::string fileName) override;
        void        Free() override;
        const char* GetPath() const override;
        const char* GetExt() const override;
        const char* GetType() const override;
    };

    CS_DECLARE_ASSET(sfx::Music)
    {
    public:
        sfx::Music m_data;

        bool        Load(std::string fileName) override;
        void        Free() override;
        const char* GetPath() const override;
        const char* GetExt() const override;
        const char* GetType() const override;
    };
}
