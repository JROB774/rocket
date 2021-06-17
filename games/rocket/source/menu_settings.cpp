static void SettingsMenuActionSound(MenuOption& option)
{
    sfx::SetSoundVolume(option.slider);
}

static void SettingsMenuActionMusic(MenuOption& option)
{
    sfx::SetMusicVolume(option.slider);
}

static void SettingsMenuActionFullscreen(MenuOption& option)
{
    FullscreenWindow(option.toggle);
}

static void SettingsMenuActionVSync(MenuOption& option)
{
    EnableVSync(option.toggle);
}

static void SettingsMenuActionResetSave(MenuOption& option)
{
    s_resetSaveCounter++;
    if(s_resetSaveCounter >= 3)
    {
        s_resetSaveCounter = 0;
        sfx::PlaySound("reset");
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
{ SettingsMenuActionSound,      MenuOptionType_Slider, { 0.0f,128.0f,      180.0f,24.0f }, { 0,1128,128,24 } },
{ SettingsMenuActionMusic,      MenuOptionType_Slider, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0,1392,128,24 } },
{ SettingsMenuActionFullscreen, MenuOptionType_Toggle, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0, 408,128,24 } },
{ SettingsMenuActionVSync,      MenuOptionType_Toggle, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0, 456,128,24 } },
{ SettingsMenuActionResetSave,  MenuOptionType_Button, { 0.0f,128.0f+96.0f,180.0f,24.0f }, { 0, 504,128,24 } },
{ SettingsMenuActionBack,       MenuOptionType_Button, { 0.0f,288.0f,      180.0f,24.0f }, { 0, 576,128,24 } }
};

static void UpdateSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    s_settingsMenuOptions[SettingsMenuOption_Sound].slider = sfx::GetSoundVolume();
    s_settingsMenuOptions[SettingsMenuOption_Music].slider = sfx::GetMusicVolume();
    s_settingsMenuOptions[SettingsMenuOption_Fullscreen].toggle = IsFullscreen();
    s_settingsMenuOptions[SettingsMenuOption_VSync].toggle = IsVSyncOn();
    s_settingsMenuOptions[SettingsMenuOption_ResetSave].clip.y = 504+(CS_CAST(f32,s_resetSaveCounter)*24.0f);
    UpdateMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        SettingsMenuActionBack(s_settingsMenuOptions[SettingsMenuOption_Back]);
}

static void RenderSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    RenderMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    Rect titleClip = { 0,128,256,32 };
    imm::DrawTexture("menu", gfx::GetScreenWidth()*0.5f,24.0f, &titleClip);
}

static void GoToSettingsMenu()
{
    s_gameState = GameState_SettingsMenu;
    ResetMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL);
    s_resetSaveCounter = 0;
}