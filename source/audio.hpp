typedef u32 SoundRef;

DECLARE_PRIVATE_STRUCT(Sound);
DECLARE_PRIVATE_STRUCT(Music);

static const SoundRef k_invalidSoundRef = 0xFFFFFFFF;

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
static bool LoadSound(Sound& sound, std::string fileNmae);
static void FreeSound(Sound& sound);
static SoundRef PlaySound(std::string soundName, s32 loops = 0);
static SoundRef PlaySound(Sound sound, s32 loops = 0);
static void StopSound(SoundRef soundRef);

// Music
static bool LoadMusic(Music& music, std::string fileName);
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

    bool        Load(std::string fileName) override;
    void        Free() override;
    const char* GetPath() const override;
    const char* GetExt() const override;
    const char* GetType() const override;
};

DECLARE_ASSET(Music)
{
public:
    Music m_data;

    bool        Load(std::string fileName) override;
    void        Free() override;
    const char* GetPath() const override;
    const char* GetExt() const override;
    const char* GetType() const override;
};
