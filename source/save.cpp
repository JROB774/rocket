// The save file format is as follows:
//
// u16     - Version Number
// u8      - Current Costume
// u8      - Costume Unlocks as Bitflags
// u32[10] - Highscores

static void SaveGame()
{
    #ifdef __EMSCRIPTEN__
    std::string fileName = "/ROCKET/";
    #else
    std::string fileName = GetExecPath();
    #endif

    fileName += k_saveFileName;

    FILE* file = fopen(fileName.c_str(), "wb");
    if(!file)
        printf("Failed to save game data!\n");
    else
    {
        u8 currentCostume = (s_rocket.random) ? NK_CAST(u8, Costume_Random) : NK_CAST(u8, s_rocket.costume);
        u8 unlockFlags = UnlockFlags_None;

        if(s_rocket.unlocks[Costume_Happy  ]) SET_FLAGS(unlockFlags, UnlockFlags_Happy);
        if(s_rocket.unlocks[Costume_Sad    ]) SET_FLAGS(unlockFlags, UnlockFlags_Sad);
        if(s_rocket.unlocks[Costume_Sick   ]) SET_FLAGS(unlockFlags, UnlockFlags_Sick);
        if(s_rocket.unlocks[Costume_Meat   ]) SET_FLAGS(unlockFlags, UnlockFlags_Meat);
        if(s_rocket.unlocks[Costume_Doodle ]) SET_FLAGS(unlockFlags, UnlockFlags_Doodle);
        if(s_rocket.unlocks[Costume_Rainbow]) SET_FLAGS(unlockFlags, UnlockFlags_Rainbow);
        if(s_rocket.unlocks[Costume_Glitch ]) SET_FLAGS(unlockFlags, UnlockFlags_Glitch);

        fwrite(&k_saveVersion, sizeof(k_saveVersion), 1, file);
        fwrite(&currentCostume, sizeof(currentCostume), 1, file);
        fwrite(&unlockFlags, sizeof(unlockFlags), 1, file);
        for(s32 i=0; i<10; ++i)
            fwrite(&s_rocket.highscores[i], sizeof(s_rocket.highscores[i]), 1, file);

        fclose(file);

        #ifdef __EMSCRIPTEN__
        EM_ASM(FS.syncfs(function(err) { assert(!err); }));
        #endif // __EMSCRIPTEN__
    }
}

static void LoadGame()
{
    // These rockets are always unlocked.
    s_rocket.unlocks[Costume_Red   ] = true;
    s_rocket.unlocks[Costume_Blue  ] = true;
    s_rocket.unlocks[Costume_Yellow] = true;
    s_rocket.unlocks[Costume_Random] = true;

    // Load data if available.
    #ifdef __EMSCRIPTEN__
    std::string fileName = "/ROCKET/";
    #else
    std::string fileName = GetExecPath();
    #endif

    fileName += k_saveFileName;

    if(DoesFileExist(fileName))
    {
        FILE* file = fopen(fileName.c_str(), "rb");
        if(!file)
            printf("Failed to load game data!\n");
        else
        {
            u16 version;
            u8 currentCostume;
            u8 unlockFlags;

            fread(&version, sizeof(version), 1, file);
            if(version == 0)
            {
                fread(&currentCostume, sizeof(currentCostume), 1, file);
                fread(&unlockFlags, sizeof(unlockFlags), 1, file);
                for(s32 i=0; i<10; ++i)
                    fread(&s_rocket.highscores[i], sizeof(s_rocket.highscores[i]), 1, file);

                s_rocket.costume = NK_CAST(Costume, currentCostume);
                s_rocket.random = (s_rocket.costume == Costume_Random);

                s_rocket.unlocks[Costume_Happy  ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Happy);
                s_rocket.unlocks[Costume_Sad    ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Sad);
                s_rocket.unlocks[Costume_Sick   ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Sick);
                s_rocket.unlocks[Costume_Meat   ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Meat);
                s_rocket.unlocks[Costume_Doodle ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Doodle);
                s_rocket.unlocks[Costume_Rainbow] = CHECK_FLAGS(unlockFlags, UnlockFlags_Rainbow);
                s_rocket.unlocks[Costume_Glitch ] = CHECK_FLAGS(unlockFlags, UnlockFlags_Glitch);

                fclose(file);

                #ifdef __EMSCRIPTEN__
                EM_ASM(FS.syncfs(function(err) { assert(!err); }));
                #endif // __EMSCRIPTEN__
            }
        }
    }
}

static void ResetSave()
{
    s_rocket.unlocks[Costume_Happy  ] = false;
    s_rocket.unlocks[Costume_Sad    ] = false;
    s_rocket.unlocks[Costume_Sick   ] = false;
    s_rocket.unlocks[Costume_Meat   ] = false;
    s_rocket.unlocks[Costume_Doodle ] = false;
    s_rocket.unlocks[Costume_Rainbow] = false;
    s_rocket.unlocks[Costume_Glitch ] = false;
    for(s32 i=0; i<10; ++i)
        s_rocket.highscores[i] = 0;
    s_rocket.costume = Costume_Red;
}
