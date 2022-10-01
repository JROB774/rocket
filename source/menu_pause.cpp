static void PauseMenuActionResume(MenuOption& option)
{
    ResumeGame();
}

static void PauseMenuActionSettings(MenuOption& option)
{
    GoToSettingsMenu();
}

static void PauseMenuActionMenu(MenuOption& option)
{
    ResetGame(GameState_MainMenu);
    ResumeMusic();
}

static MenuOption s_pauseMenuOptions[PauseMenuOption_TOTAL]
{
MenuOption(PauseMenuActionResume,   MenuOptionType_Button, { 0.0f,200.0f,180.0f,24.0f }, { 0,312,128,24 }),
MenuOption(MainMenuActionSettings,  MenuOptionType_Button, { 0.0f,224.0f,180.0f,24.0f }, { 0,264,128,24 }),
MenuOption(PauseMenuActionMenu,     MenuOptionType_Button, { 0.0f,248.0f,180.0f,24.0f }, { 0,336,128,24 })
};

static void PauseGame()
{
    s_gamePaused = true;
    PlaySound("pause");
    PauseMusic();
    StopThruster();
    GoToPauseMenu();
}

static void ResumeGame()
{
    s_gamePaused = false;
    PlaySound("pause");
    ResumeMusic();
    StartThruster();
}

static void UpdatePauseMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || s_rocket.dead) return;

    // Toggle pause menu.
    if(IsKeyPressed(KeyCode_Escape))
    {
        s_gamePaused = !s_gamePaused;
        if(!s_gamePaused) PauseMenuActionResume(s_pauseMenuOptions[PauseMenuOption_Resume]);
        else PauseGame();
    }

    // Do pause menu.
    if(s_gamePaused)
    {
        UpdateMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL, dt);
    }
}

static void RenderPauseMenu(f32 dt)
{
    if(!s_gamePaused) return;

    f32 screenW = GetScreenWidth();
    f32 screenH = GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    imm::DrawRectFilled(0,0,screenW,screenH, { 0,0,0,0.5f });

    if(s_gameState != GameState_Game) return;

    static Rect s_pauseClip = { 0,160,256,32 };

    imm::DrawTexture("menu", halfW,halfH-40, &s_pauseClip);
    RenderMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL, dt);
}

static void GoToPauseMenu()
{
    s_gameState = GameState_Game;
    ResetMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL);
}
