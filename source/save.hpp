#pragma once

static constexpr const char* k_saveFileName = "save.dat";
static constexpr u16 k_saveVersion = 0;

enum UnlocksFlags : u8
{
    UnlockFlags_None    = 0,
    UnlockFlags_Happy   = 1 << 0,
    UnlockFlags_Sad     = 1 << 1,
    UnlockFlags_Sick    = 1 << 2,
    UnlockFlags_Meat    = 1 << 3,
    UnlockFlags_Doodle  = 1 << 4,
    UnlockFlags_Rainbow = 1 << 5,
    UnlockFlags_Glitch  = 1 << 6
};

static void SaveGame();
static void LoadGame();
static void ResetSave();
