static bool s_hasDoneInitialGameOverUnlockClick;
static f32  s_unlockCostumeScale;
static f32  s_unlockCostumeAngle;
static f32  s_unlockRadialAngle;
static f32  s_unlockRadialAlpha;
static f32  s_unlockRadialScale;

static void GameOverMenuActionRetry(MenuOption& option)
{
    ResetGame(GameState_Game);
}

static void GameOverMenuActionMenu(MenuOption& option)
{
    ResetGame(GameState_MainMenu);
}

static MenuOption s_gameOverMenuOptions[GameOverMenuOption_TOTAL]
{
MenuOption(GameOverMenuActionRetry, MenuOptionType_Button, { 0.0f,248.0f,180.0f,24.0f }, { 0,1656,128,24 }),
MenuOption(GameOverMenuActionMenu,  MenuOptionType_Button, { 0.0f,272.0f,180.0f,24.0f }, { 0, 336,128,24 })
};

static void SetupNextUnlockState()
{
    PlaySound("unlock");
    s_unlockCostumeScale = 0.0f;
    s_unlockRadialAlpha = 0.0f;
    s_unlockRadialScale = 0.0f;
}

static void UpdateGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;

    // If we have unlocked stuff display that first before the actual game over menu.
    if(!s_gameOverUnlocks.empty())
    {
        if(!s_hasDoneInitialGameOverUnlockClick)
        {
            s_hasDoneInitialGameOverUnlockClick = IsMouseButtonPressed(MouseButton_Left);
            if(s_hasDoneInitialGameOverUnlockClick) SetupNextUnlockState();
        }
        else
        {
            // Move on to the next unlock, or the game over menu.
            if(IsMouseButtonPressed(MouseButton_Left))
            {
                s_gameOverUnlocks.erase(s_gameOverUnlocks.begin());
                if(!s_gameOverUnlocks.empty()) SetupNextUnlockState();
            }
        }
    }
    else
    {
        UpdateMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL, dt);
        if(IsKeyPressed(KeyCode_R))
            GameOverMenuActionRetry(s_gameOverMenuOptions[GameOverMenuOption_Retry]);
    }
}

static void RenderGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;

    static f32 s_timer = 0.0f;

    s_timer += dt;

    s_unlockCostumeAngle = nk_sin_range(-10.0f, 10.0f, s_timer*2.5f);
    s_unlockRadialAngle += (dt * 2.0f);

    // If we have unlocked stuff display that first before the actual game over menu.
    if(!s_gameOverUnlocks.empty())
    {
        if(s_hasDoneInitialGameOverUnlockClick)
        {
            s_unlockCostumeScale = nk_lerpf(s_unlockCostumeScale, 2.0f, dt * 12.0f);
            s_unlockRadialScale  = nk_lerpf(s_unlockRadialAlpha,  1.0f, dt *  9.0f);
            s_unlockRadialAlpha  = nk_lerpf(s_unlockRadialAlpha,  1.0f, dt *  5.0f);

            Costume costume = s_gameOverUnlocks[0];

            f32 halfW = GetScreenWidth() * 0.5f;
            f32 halfH = GetScreenHeight() * 0.5f;

            f32 costumeOffset = 64 * NK_CAST(f32, costume);
            f32 nameOffset = 24 * NK_CAST(f32, costume);

            Rect costumeClip = { 64+costumeOffset,0,64,64 };
            Rect nameClip = { 0,624+nameOffset,256,24 };
            Rect unlockClip = { 0,1784,256,24 };

            nkVec4 color = { 1,1,1,s_unlockRadialAlpha };

            imm::DrawTexture("radial", halfW,halfH, s_unlockRadialScale,s_unlockRadialScale, s_unlockRadialAngle, imm::Flip_None, NULL, NULL, color);
            imm::DrawTexture("costume", halfW,halfH, s_unlockCostumeScale,s_unlockCostumeScale, nk_torad(s_unlockCostumeAngle), imm::Flip_None, NULL, &costumeClip);
            imm::DrawTexture("menu", halfW,halfH + 80.0f, &nameClip);
            imm::DrawTexture("menu", halfW,halfH + 96.0f, &unlockClip);
        }
    }
    else
    {
        RenderMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL, dt);
        Rect titleClip = { 0,1704,256,32 };
        imm::DrawTexture("menu", GetScreenWidth()*0.5f,24.0f, &titleClip);

        // Draw the score achieved.
        bool newHighscore = (s_rocket.score >= s_rocket.highscores[0]);
        std::string scoreStr = std::to_string(s_rocket.score) + "!";
        f32 textWidth = GetTextLineWidth(s_bigFont0, scoreStr);
        f32 screenWidth = GetScreenWidth();
        f32 screenHeight = GetScreenHeight();
        f32 textHeight = screenHeight*0.33f;
        DrawBitmapFont(s_bigFont0, roundf((screenWidth-textWidth)*0.5f),textHeight, scoreStr);

        // Draw a message depending on what the score was.
        Rect newHighClip  = { 0,1680,256,24 };
        Rect wellDoneClip = { 0,1760,256,24 };
        Rect niceTryClip  = { 0,1736,256,24 };

        Rect clip = niceTryClip;
        if(s_rocket.score >= s_rocket.highscores[9]) clip = wellDoneClip;
        if(s_rocket.score >= s_rocket.highscores[0]) clip = newHighClip;

        imm::DrawTexture("menu", screenWidth*0.5f,textHeight+64.0f, &clip);
    }
}

static void GoToGameOverMenu()
{
    s_gameState = GameState_Game;
    ResetMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL);
    s_hasDoneInitialGameOverUnlockClick = false;
}
