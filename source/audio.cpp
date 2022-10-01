DEFINE_PRIVATE_STRUCT(Sound)
{
    Mix_Chunk* chunk;
};

DEFINE_PRIVATE_STRUCT(Music)
{
    Mix_Music* music;
};

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

static void InitAudio()
{
    if(!(Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG))
        FatalError("Failed to initialize SDL2 Mixer OGG support! (%s)\n", Mix_GetError());
    if(Mix_OpenAudio(k_mixerFrequency, k_mixerSampleFormat, k_mixerChannels, k_mixerSampleSize) != 0)
        FatalError("Failed to open SDL2 Mixer audio device! (%s)\n", Mix_GetError());
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

static void QuitAudio()
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

static void SetSoundVolume(f32 volume)
{
    s_audioContext.soundVolume = nk::clamp(volume, 0.0f, 1.0f);
    s32 iVolume = CAST(s32, CAST(f32, MIX_MAX_VOLUME) * s_audioContext.soundVolume);
    Mix_Volume(-1, iVolume);
}

static void SetMusicVolume(f32 volume)
{
    s_audioContext.musicVolume = nk::clamp(volume, 0.0f, 1.0f);
    s32 iVolume = CAST(int, CAST(f32, MIX_MAX_VOLUME) * s_audioContext.musicVolume);
    Mix_VolumeMusic(iVolume);
}

static f32 GetSoundVolume()
{
    return s_audioContext.soundVolume;
}

static f32 GetMusicVolume()
{
    return s_audioContext.musicVolume;
}

static bool IsSoundOn()
{
    return (s_audioContext.soundVolume > 0.0f);
}

static bool IsMusicOn()
{
    return (s_audioContext.musicVolume > 0.0f);
}

static bool IsMusicPlaying()
{
    return Mix_PlayingMusic();
}

//
// Sound
//

static bool LoadSound(Sound& sound, std::string fileName)
{
    sound = Allocate<GET_PTR_TYPE(sound)>(MEM_SYSTEM);
    if(!sound) FatalError("Failed to allocate sound!\n");

    sound->chunk = Mix_LoadWAV(fileName.c_str());
    if(!sound->chunk)
        FatalError("Failed to load sound: %s (%s)\n", fileName.c_str(), Mix_GetError());
    return true;
}

static void FreeSound(Sound& sound)
{
    Mix_FreeChunk(sound->chunk);
    Deallocate(sound);
}

static SoundRef PlaySound(std::string soundName, s32 loops)
{
    Sound sound = *GetAsset<Sound>(soundName);
    if(sound) return PlaySound(sound, loops);
    return k_invalidSoundRef;
}

static SoundRef PlaySound(Sound sound, s32 loops)
{
    if(!sound) return k_invalidSoundRef;
    s32 channel = Mix_PlayChannel(-1, sound->chunk, loops);
    if(channel == -1)
    {
        printf("Failed to play sound effect! (%s)\n", Mix_GetError());
        return k_invalidSoundRef;
    }
    return CAST(SoundRef,channel);
}

static void StopSound(SoundRef soundRef)
{
    Mix_HaltChannel(soundRef);
}

//
// Music
//

static bool LoadMusic(Music& music, std::string fileName)
{
    music = Allocate<GET_PTR_TYPE(music)>(MEM_SYSTEM);
    if(!music) FatalError("Failed to allocate music!\n");
    music->music = Mix_LoadMUS(fileName.c_str());
    if(!music->music)
        FatalError("Failed to load music: %s (%s)\n", fileName.c_str(), Mix_GetError());
    return true;
}

static void FreeMusic(Music& music)
{
    Mix_FreeMusic(music->music);
    Deallocate(music);
}

static void PlayMusic(std::string musicName, s32 loops)
{
    Music music = *GetAsset<Music>(musicName);
    if(music) PlayMusic(music, loops);
}

static void PlayMusic(Music music, s32 loops)
{
    if(!music) return;
    if(Mix_PlayMusic(music->music, loops) == -1)
        printf("Failed to play music! (%s)\n", Mix_GetError());
}

static void ResumeMusic()
{
    Mix_ResumeMusic();
}

static void PauseMusic()
{
    Mix_PauseMusic();
}

static void StopMusic()
{
    Mix_HaltMusic();
}
