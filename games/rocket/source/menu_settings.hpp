#pragma once

static s32 s_resetSaveCounter;

enum SettingsMenuOption
{
    SettingsMenuOption_Sound,
    SettingsMenuOption_Music,
    SettingsMenuOption_Fullscreen,
    SettingsMenuOption_VSync,
    SettingsMenuOption_ResetSave,
    SettingsMenuOption_Back,
    SettingsMenuOption_TOTAL
};

static void UpdateSettingsMenu(f32 dt);
static void RenderSettingsMenu(f32 dt);
static void GoToSettingsMenu();
