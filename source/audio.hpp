typedef u32 SoundRef;

DECLARE_PRIVATE_STRUCT(Sound);
DECLARE_PRIVATE_STRUCT(Music);

static constexpr f32 k_defaultSoundVolume = 0.8f;
static constexpr f32 k_defaultMusicVolume = 0.6f;

static constexpr SoundRef k_invalidSoundRef = 0xFFFFFFFF;

static void InitAudio();
static void QuitAudio();

static void SetSoundVolume(f32 volume); // [0-1]
static void SetMusicVolume(f32 volume); // [0-1]

static f32 GetSoundVolume();
static f32 GetMusicVolume();

static bool IsSoundOn();
static bool IsMusicOn();

static bool IsMusicPlaying();

// Sound
static bool LoadSoundFromFile(Sound& sound, std::string fileNmae);
static bool LoadSoundFromData(Sound& sound, void* data, size_t bytes);
static void FreeSound(Sound& sound);
static SoundRef PlaySound(std::string soundName, s32 loops = 0);
static SoundRef PlaySound(Sound sound, s32 loops = 0);
static void StopSound(SoundRef soundRef);

// Music
static bool LoadMusicFromFile(Music& music, std::string fileName);
static bool LoadMusicFromData(Music& music, void* data, size_t bytes);
static void FreeMusic(Music& music);
static void PlayMusic(std::string musicName, s32 loops = 0);
static void PlayMusic(Music music, s32 loops = 0);
static void ResumeMusic();
static void PauseMusic();
static void StopMusic();

// Assets
DECLARE_ASSET(Sound)
{
public:
    Sound m_data;

    bool        LoadFromFile(std::string fileName) override { return LoadSoundFromFile(m_data, fileName); }
    bool        LoadFromData(void* data, size_t bytes) override { return LoadSoundFromData(m_data, data, bytes); }
    void        Free() override { FreeSound(m_data); }
    const char* GetPath() const override { return "sounds/"; }
    const char* GetExt() const override { return ".ogg"; }
    const char* GetType() const override { return "Sound"; }
};

DECLARE_ASSET(Music)
{
public:
    Music m_data;

    bool        LoadFromFile(std::string fileName) override { return LoadMusicFromFile(m_data, fileName); }
    bool        LoadFromData(void* data, size_t bytes) override { return LoadMusicFromData(m_data, data, bytes); }
    void        Free() override { FreeMusic(m_data); }
    const char* GetPath() const override { return "music/"; }
    const char* GetExt() const override { return ".ogg"; }
    const char* GetType() const override { return "Music"; }
};
