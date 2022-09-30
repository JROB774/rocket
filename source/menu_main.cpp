static void MainMenuActionStart(MenuOption& option)
{
    s_entitySpawnCooldown = k_entitySpawnCooldownTime;
    s_entitySpawnTimer = 0.0f;
    ResetGame(GameState_Game);
}

static void MainMenuActionScores(MenuOption& option)
{
    GoToScoresMenu();
}

static void MainMenuActionCostumes(MenuOption& option)
{
    GoToCostumesMenu();
}

static void MainMenuActionSettings(MenuOption& option)
{
    GoToSettingsMenu();
}

static void MainMenuActionExit(MenuOption& option)
{
    GetAppConfig().app->m_running = false;
}

static MenuOption s_mainMenuOptions[MainMenuOption_TOTAL]
{
{ MainMenuActionStart,    MenuOptionType_Button, { 0.0f,128.0f,      180.0f,24.0f }, { 0, 192,128,24 } },
{ MainMenuActionScores,   MenuOptionType_Button, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0, 216,128,24 } },
{ MainMenuActionCostumes, MenuOptionType_Button, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0, 240,128,24 } },
{ MainMenuActionSettings, MenuOptionType_Button, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0, 264,128,24 } },
{ MainMenuActionExit,     MenuOptionType_Button, { 0.0f,128.0f+96.0f,180.0f,24.0f }, { 0, 288,128,24 } }
};

static void UpdateMainMenu(f32 dt)
{
    if(s_gameState != GameState_MainMenu) return;
    UpdateMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        MainMenuActionExit(s_mainMenuOptions[MainMenuOption_Exit]);
}

static void RenderMainMenu(f32 dt)
{
    if(s_gameState != GameState_MainMenu) return;
    RenderMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL, dt);

    Rect titleClip  = { 0,   0,256,64 };
    Rect authorClip = { 0,1056,256,24 };

    static f32 s_scaleX = 1.0f;
    static f32 s_scaleY = 1.0f;
    static f32 s_angle  = 0.0f;
    static f32 s_timer  = 0.0f;

    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    s_scaleX = SinRange(0.8f, 1.0f, s_timer*1.5f);
    s_scaleY = SinRange(0.8f, 1.0f, s_timer*2.0f);

    imm::DrawTexture("menu", halfW,48.0f, s_scaleX,s_scaleY, csm::ToRad(s_angle), imm::Flip_None, NULL, &titleClip);
    imm::DrawTexture("menu", halfW,screenH-12.0f, &authorClip);
}

static void GoToMainMenu()
{
    s_gameState = GameState_MainMenu;
    ResetMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL);
}