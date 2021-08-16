#include "cs_audio.hpp"

using namespace cs;
using namespace sfx;

CS_PUBLIC_SCOPE::sfx
{
    CS_DEFINE_PRIVATE_STRUCT(Sound) {};
    CS_DEFINE_PRIVATE_STRUCT(Music) {};

    CS_API void InitAudio()
    {
        // Nothing...
    }

    CS_API void QuitAudio()
    {
        // Nothing...
    }

    CS_API void SetSoundVolume(f32 volume)
    {
        // Nothing...
    }

    CS_API void SetMusicVolume(f32 volume)
    {
        // Nothing...
    }

    CS_API f32 GetSoundVolume()
    {
        return 0.0f;
    }

    CS_API f32 GetMusicVolume()
    {
        return 0.0f;
    }

    CS_API bool IsSoundOn()
    {
        return false;
    }

    CS_API bool IsMusicOn()
    {
        return false;
    }

    CS_API bool IsMusicPlaying()
    {
        return false;
    }

    //
    // Sound
    //

    CS_API bool LoadSound(Sound& sound, std::string fileName)
    {
        return true;
    }

    CS_API void FreeSound(Sound& sound)
    {
        // Nothing...
    }

    CS_API SoundRef PlaySound(std::string soundName, s32 loops)
    {
        return k_invalidSoundRef;
    }

    CS_API SoundRef PlaySound(Sound sound, s32 loops)
    {
        return k_invalidSoundRef;
    }

    CS_API void StopSound(SoundRef sound)
    {
        // Nothing...
    }

    //
    // Music
    //

    CS_API bool LoadMusic(Music& music, std::string fileName)
    {
        return true;
    }

    CS_API void FreeMusic(Music& music)
    {
        // Nothing...
    }

    CS_API void PlayMusic(std::string musicName, s32 loops)
    {
        // Nothing...
    }

    CS_API void PlayMusic(Music music, s32 loops)
    {
        // Nothing...
    }

    CS_API void ResumeMusic()
    {
        // Nothing...
    }

    CS_API void PauseMusic()
    {
        // Nothing...
    }

    CS_API void StopMusic()
    {
        // Nothing...
    }
}

CS_PUBLIC_SCOPE
{
    bool Asset<sfx::Sound>::Load(std::string fileName)
    {
        return true;
    }
    void Asset<sfx::Sound>::Free()
    {
        // Nothing...
    }
    void Asset<sfx::Sound>::DoDebugView()
    {
        // Nothing...
    }
    const char* Asset<sfx::Sound>::GetPath() const
    {
        return "sounds/";
    }
    const char* Asset<sfx::Sound>::GetExt() const
    {
        return ".ogg";
    }
    const char* Asset<sfx::Sound>::GetType() const
    {
        return "Sound";
    }

    bool Asset<sfx::Music>::Load(std::string fileName)
    {
        return true;
    }
    void Asset<sfx::Music>::Free()
    {
        // Nothing...
    }
    void Asset<sfx::Music>::DoDebugView()
    {
        // Nothing...
    }
    const char* Asset<sfx::Music>::GetPath() const
    {
        return "music/";
    }
    const char* Asset<sfx::Music>::GetExt() const
    {
        return ".wav";
    }
    const char* Asset<sfx::Music>::GetType() const
    {
        return "Music";
    }
}
