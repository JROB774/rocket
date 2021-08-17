// The save file format is as follows:
//
// u16     - Version Number
// u8      - Current Costume
// u8      - Costume Unlocks as Bitflags
// u32[10] - Highscores

static void SaveGame()
{
    std::string fileName = GetDataPath() + k_saveFileName;
    FILE* file = fopen(fileName.c_str(), "wb");
    if(!file)
        CS_DEBUG_LOG("Failed to save game data!");
    else
    {
        CS_DEFER { fclose(file); };

        u8 currentCostume = (s_rocket.random) ? CS_CAST(u8, Costume_Random) : CS_CAST(u8, s_rocket.costume);
        u8 unlockFlags = UnlockFlags_None;

        if(s_rocket.unlocks[Costume_Happy  ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Happy);
        if(s_rocket.unlocks[Costume_Sad    ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Sad);
        if(s_rocket.unlocks[Costume_Sick   ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Sick);
        if(s_rocket.unlocks[Costume_Meat   ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Meat);
        if(s_rocket.unlocks[Costume_Doodle ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Doodle);
        if(s_rocket.unlocks[Costume_Rainbow]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Rainbow);
        if(s_rocket.unlocks[Costume_Glitch ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Glitch);

        fwrite(&k_saveVersion, sizeof(k_saveVersion), 1, file);
        fwrite(&currentCostume, sizeof(currentCostume), 1, file);
        fwrite(&unlockFlags, sizeof(unlockFlags), 1, file);
        for(s32 i=0; i<10; ++i)
            fwrite(&s_rocket.highscores[i], sizeof(s_rocket.highscores[i]), 1, file);
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
    std::string fileName = GetDataPath() + k_saveFileName;
    if(DoesFileExist(fileName))
    {
        FILE* file = fopen(fileName.c_str(), "rb");
        if(!file)
            CS_DEBUG_LOG("Failed to load game data!");
        else
        {
            CS_DEFER { fclose(file); };

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

                s_rocket.costume = CS_CAST(Costume, currentCostume);
                s_rocket.random = (s_rocket.costume == Costume_Random);

                s_rocket.unlocks[Costume_Happy  ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Happy);
                s_rocket.unlocks[Costume_Sad    ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Sad);
                s_rocket.unlocks[Costume_Sick   ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Sick);
                s_rocket.unlocks[Costume_Meat   ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Meat);
                s_rocket.unlocks[Costume_Doodle ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Doodle);
                s_rocket.unlocks[Costume_Rainbow] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Rainbow);
                s_rocket.unlocks[Costume_Glitch ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Glitch);
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
