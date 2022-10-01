#include "cs_audio.hpp"
#include "cs_platform.hpp"
#include "cs_utility.hpp"

#include <SDL_mixer.h>

using namespace cs;
using namespace sfx;

CS_PUBLIC_SCOPE::sfx
{
    DEFINE_PRIVATE_STRUCT(Sound)
    {
        Mix_Chunk* chunk;
    };

    DEFINE_PRIVATE_STRUCT(Music)
    {
        Mix_Music* music;
    };
}

CS_PRIVATE_SCOPE
{
    static constexpr s32 k_mixerFrequency = MIX_DEFAULT_FREQUENCY;
    static constexpr u16 k_mixerSampleFormat = MIX_DEFAULT_FORMAT;
    static constexpr s32 k_mixerChannels = 2; // Stereo Sound
    static constexpr s32 k_mixerSampleSize = 2048;

    static constexpr f32 k_defaultSoundVolume = 0.5f;
    static constexpr f32 k_defaultMusicVolume = 0.5f;

    struct AudioContext
    {
        f32 soundVolume;
        f32 musicVolume;
    };

    static AudioContext s_audioContext;
}

CS_PUBLIC_SCOPE::sfx
{
    CS_API void InitAudio()
    {
        if(!(Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG))
            printf("Failed to initialize SDL2 Mixer OGG support! (%s)\n", Mix_GetError()); // @Incomplete: Fatal error should terminate!
        if(Mix_OpenAudio(k_mixerFrequency, k_mixerSampleFormat, k_mixerChannels, k_mixerSampleSize) != 0)
            printf("Failed to open SDL2 Mixer audio device! (%s)\n", Mix_GetError()); // @Incomplete: Fatal error should terminate!
        Mix_AllocateChannels(32);

        f32 soundVolume = k_defaultSoundVolume;
        f32 musicVolume = k_defaultMusicVolume;

        // @Incomplete: Need to reimplement sound/music volume saving!
        /*
        json& engineState = GetState();
        if(engineState.contains("audio"))
        {
            json& audioState = engineState["audio"];
            soundVolume = GetJSONValueWithDefault<f32>(audioState, "sound_volume", k_defaultSoundVolume);
            musicVolume = GetJSONValueWithDefault<f32>(audioState, "music_volume", k_defaultMusicVolume);
        }
        */

        SetSoundVolume(soundVolume);
        SetMusicVolume(musicVolume);
    }

    CS_API void QuitAudio()
    {
        Mix_CloseAudio();

        // @Incomplete: Need to reimplement sound/music volume loading!
        /*
        json& audioState = json::object();
        audioState["sound_volume"] = s_audioContext.soundVolume;
        audioState["music_volume"] = s_audioContext.musicVolume;
        GetState()["audio"] = audioState;
        */
    }

    CS_API void SetSoundVolume(f32 volume)
    {
        s_audioContext.soundVolume = csm::Clamp(volume, 0.0f, 1.0f);
        s32 iVolume = CS_CAST(s32, CS_CAST(f32, MIX_MAX_VOLUME) * s_audioContext.soundVolume);
        Mix_Volume(-1, iVolume);
    }

    CS_API void SetMusicVolume(f32 volume)
    {
        s_audioContext.musicVolume = csm::Clamp(volume, 0.0f, 1.0f);
        s32 iVolume = CS_CAST(int, CS_CAST(f32, MIX_MAX_VOLUME) * s_audioContext.musicVolume);
        Mix_VolumeMusic(iVolume);
    }

    CS_API f32 GetSoundVolume()
    {
        return s_audioContext.soundVolume;
    }

    CS_API f32 GetMusicVolume()
    {
        return s_audioContext.musicVolume;
    }

    CS_API bool IsSoundOn()
    {
        return (s_audioContext.soundVolume > 0.0f);
    }

    CS_API bool IsMusicOn()
    {
        return (s_audioContext.musicVolume > 0.0f);
    }

    CS_API bool IsMusicPlaying()
    {
        return Mix_PlayingMusic();
    }

    //
    // Sound
    //

    CS_API bool LoadSound(Sound& sound, std::string fileName)
    {
        sound = Allocate<CS_GET_PTR_TYPE(sound)>(CS_MEM_SYSTEM);
        if(!sound) printf("Failed to allocate sound!\n"); // @Incomplete: Fatal error should terminate!

        sound->chunk = Mix_LoadWAV(fileName.c_str());
        if(!sound->chunk)
            printf("Failed to load sound: %s (%s)\n", fileName.c_str(), Mix_GetError()); // @Incomplete: Fatal error should terminate!
        return true;
    }

    CS_API void FreeSound(Sound& sound)
    {
        Mix_FreeChunk(sound->chunk);
        Deallocate(sound);
    }

    CS_API SoundRef PlaySound(std::string soundName, s32 loops)
    {
        Sound sound = *GetAsset<Sound>(soundName);
        if(sound) return PlaySound(sound, loops);
        return k_invalidSoundRef;
    }

    CS_API SoundRef PlaySound(Sound sound, s32 loops)
    {
        if(!sound) return k_invalidSoundRef;
        s32 channel = Mix_PlayChannel(-1, sound->chunk, loops);
        if(channel == -1)
        {
            printf("Failed to play sound effect! (%s)\n", Mix_GetError());
            return k_invalidSoundRef;
        }
        return CS_CAST(SoundRef,channel);
    }

    CS_API void StopSound(SoundRef soundRef)
    {
        Mix_HaltChannel(soundRef);
    }

    //
    // Music
    //

    CS_API bool LoadMusic(Music& music, std::string fileName)
    {
        music = Allocate<CS_GET_PTR_TYPE(music)>(CS_MEM_SYSTEM);
        if(!music) printf("Failed to allocate music!\n"); // @Incomplete: Fatal error should terminate!

        music->music = Mix_LoadMUS(fileName.c_str());
        if(!music->music)
            printf("Failed to load music: %s (%s)\n", fileName.c_str(), Mix_GetError()); // @Incomplete: Fatal error should terminate!
        return true;
    }

    CS_API void FreeMusic(Music& music)
    {
        Mix_FreeMusic(music->music);
        Deallocate(music);
    }

    CS_API void PlayMusic(std::string musicName, s32 loops)
    {
        Music music = *GetAsset<Music>(musicName);
        if(music) PlayMusic(music, loops);
    }

    CS_API void PlayMusic(Music music, s32 loops)
    {
        if(!music) return;
        if(Mix_PlayMusic(music->music, loops) == -1)
            printf("Failed to play music! (%s)\n", Mix_GetError());
    }

    CS_API void ResumeMusic()
    {
        Mix_ResumeMusic();
    }

    CS_API void PauseMusic()
    {
        Mix_PauseMusic();
    }

    CS_API void StopMusic()
    {
        Mix_HaltMusic();
    }
}

CS_PUBLIC_SCOPE
{
    bool Asset<sfx::Sound>::Load(std::string fileName)
    {
        return sfx::LoadSound(m_data, fileName);
    }
    void Asset<sfx::Sound>::Free()
    {
        sfx::FreeSound(m_data);
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
        return sfx::LoadMusic(m_data, fileName);
    }
    void Asset<sfx::Music>::Free()
    {
        sfx::FreeMusic(m_data);
    }
    const char* Asset<sfx::Music>::GetPath() const
    {
        return "music/";
    }
    const char* Asset<sfx::Music>::GetExt() const
    {
        return ".ogg";
    }
    const char* Asset<sfx::Music>::GetType() const
    {
        return "Music";
    }
}
