static void ScoresMenuActionBack(MenuOption& option)
{
    GoToMainMenu();
}

static MenuOption s_scoresMenuOptions[ScoresMenuOption_TOTAL]
{
MenuOption(ScoresMenuActionBack, MenuOptionType_Button, { 0.0f,288.0f,180.0f,24.0f }, { 0,576,128,24 })
};

static void UpdateScoresMenu(f32 dt)
{
    if(s_gameState != GameState_ScoresMenu) return;
    UpdateMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        ScoresMenuActionBack(s_scoresMenuOptions[ScoresMenuOption_Back]);
}

static void RenderScoresMenu(f32 dt)
{
    if(s_gameState != GameState_ScoresMenu) return;
    RenderMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL, dt);
    Rect titleClip = { 0,64,256,32 };
    imm::DrawTexture("menu", GetScreenWidth()*0.5f,24.0f, &titleClip);

    // Draw the highscores.
    f32 screenWidth = GetScreenWidth();
    f32 screenHeight = GetScreenHeight();
    f32 yPos = 42.0f;
    for(s32 i=0; i<10; ++i)
    {
        BitmapFont* font = (i == 0) ? &s_font1 : &s_font0;
        u32 score = s_rocket.highscores[i];
        std::string scoreStr = std::to_string(score);
        f32 textWidth = GetTextLineWidth(*font, scoreStr);
        DrawBitmapFont(*font, roundf((screenWidth-textWidth)*0.5f),yPos, scoreStr);
        yPos += 24.0f;
    }
}

static void GoToScoresMenu()
{
    s_gameState = GameState_ScoresMenu;
    ResetMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL);
}
