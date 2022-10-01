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
{ GameOverMenuActionRetry, MenuOptionType_Button, { 0.0f,248.0f,180.0f,24.0f }, { 0,1656,128,24 } },
{ GameOverMenuActionMenu,  MenuOptionType_Button, { 0.0f,272.0f,180.0f,24.0f }, { 0, 336,128,24 } }
};

static void UpdateGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;
    UpdateMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_R))
        GameOverMenuActionRetry(s_gameOverMenuOptions[GameOverMenuOption_Retry]);
}

static void RenderGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;
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

static void GoToGameOverMenu()
{
    s_gameState = GameState_Game;
    ResetMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL);
}
