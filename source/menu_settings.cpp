static void SettingsMenuActionSound(MenuOption& option)
{
    SetSoundVolume(option.slider);
}

static void SettingsMenuActionMusic(MenuOption& option)
{
    SetMusicVolume(option.slider);
}

static void SettingsMenuActionFullscreen(MenuOption& option)
{
    FullscreenWindow(option.toggle);
}

static void SettingsMenuActionResetSave(MenuOption& option)
{
    s_resetSaveCounter++;
    if(s_resetSaveCounter >= 3)
    {
        s_resetSaveCounter = 0;
        PlaySound("reset");
        ResetSave();
    }
}

static void SettingsMenuActionBack(MenuOption& option)
{
    if(s_gamePaused) GoToPauseMenu();
    else GoToMainMenu();
}

static MenuOption s_settingsMenuOptions[SettingsMenuOption_TOTAL]
{
MenuOption(SettingsMenuActionSound,      MenuOptionType_Slider, { 0.0f,128.0f,      180.0f,24.0f }, { 0,1128,128,24 }),
MenuOption(SettingsMenuActionMusic,      MenuOptionType_Slider, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0,1392,128,24 }),
MenuOption(SettingsMenuActionFullscreen, MenuOptionType_Toggle, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0, 408,128,24 }),
MenuOption(SettingsMenuActionResetSave,  MenuOptionType_Button, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0, 504,128,24 }),
MenuOption(SettingsMenuActionBack,       MenuOptionType_Button, { 0.0f,288.0f,      180.0f,24.0f }, { 0, 576,128,24 })
};

static void UpdateSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    s_settingsMenuOptions[SettingsMenuOption_Sound].slider = GetSoundVolume();
    s_settingsMenuOptions[SettingsMenuOption_Music].slider = GetMusicVolume();
    s_settingsMenuOptions[SettingsMenuOption_Fullscreen].toggle = IsFullscreen();
    s_settingsMenuOptions[SettingsMenuOption_ResetSave].clip.y = 504+(CAST(f32,s_resetSaveCounter)*24.0f);
    UpdateMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        SettingsMenuActionBack(s_settingsMenuOptions[SettingsMenuOption_Back]);
}

static void RenderSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    RenderMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    Rect titleClip = { 0,128,256,32 };
    imm::DrawTexture("menu", GetScreenWidth()*0.5f,24.0f, &titleClip);
}

static void GoToSettingsMenu()
{
    s_gameState = GameState_SettingsMenu;
    ResetMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL);
    s_resetSaveCounter = 0;
}
