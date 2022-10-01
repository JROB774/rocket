static void CostumesMenuActionLeft(MenuOption& option)
{
    s32 costume = CAST(s32, s_rocket.costume);
    if(costume > 0) costume--;
    else costume = Costume_TOTAL-1;
    s_rocket.costume = CAST(Costume, costume);
    s_costumeScale = 1.5f;
}

static void CostumesMenuActionRight(MenuOption& option)
{
    s32 costume = CAST(s32, s_rocket.costume);
    if(costume < Costume_TOTAL-1) costume++;
    else costume = 0;
    s_rocket.costume = CAST(Costume, costume);
    s_costumeScale = 1.5f;
}

static void CostumesMenuActionBack(MenuOption& option)
{
    if(!s_rocket.unlocks[s_rocket.costume]) // If the selected costume is locked reset to the last costume.
        s_rocket.costume = s_currentCostume;
    s_rocket.random = (s_rocket.costume == Costume_Random);
    GoToMainMenu();
}

static MenuOption s_costumesMenuOptions[CostumesMenuOption_TOTAL]
{
MenuOption(CostumesMenuActionLeft,  MenuOptionType_Button, {   8.0f,136.0f, 48.0f,48.0f }, { 0,1080,128,24 }),
MenuOption(CostumesMenuActionRight, MenuOptionType_Button, { 124.0f,136.0f, 48.0f,48.0f }, { 0,1104,128,24 }),
MenuOption(CostumesMenuActionBack,  MenuOptionType_Button, {   0.0f,288.0f,180.0f,24.0f }, { 0, 576,128,24 })
};

static void UpdateCostumesMenu(f32 dt)
{
    if(s_gameState != GameState_CostumesMenu) return;
    UpdateMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        CostumesMenuActionBack(s_costumesMenuOptions[CostumesMenuOption_Back]);
}

static void RenderCostumesMenu(f32 dt)
{
    if(s_gameState != GameState_CostumesMenu) return;
    RenderMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL, dt);

    f32 screenW = GetScreenWidth();
    f32 screenH = GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    nkVec2 mouse = GetScreenMousePos();

    // Draw the title.
    Rect titleClip = { 0,96,256,32 };
    imm::DrawTexture("menu", halfW,24.0f, &titleClip);

    // Draw the costume.
    f32 costumeOffset = 64 * CAST(f32, s_rocket.costume);
    f32 nameOffset = 24 * CAST(f32, s_rocket.costume);

    Rect costumeClip = { 64+costumeOffset,0,64,64 };
    Rect nameClip = { 0,624+nameOffset,256,24 };

    if(!s_rocket.unlocks[s_rocket.costume])
    {
        costumeClip.x = 0.0f;
        nameClip.y += k_costumeLockedTextOffset;
    }

    s_costumeScale = nk::lerp(s_costumeScale, 1.0f, 0.5f);

    imm::DrawTexture("costume", halfW,halfH, s_costumeScale,s_costumeScale, 0.0f, imm::Flip_None, NULL, &costumeClip);
    imm::DrawTexture("menu", halfW,halfH+48, &nameClip);
}

static void GoToCostumesMenu()
{
    s_gameState = GameState_CostumesMenu;
    ResetMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL);
    if(s_rocket.random)
        s_rocket.costume = Costume_Random;
    s_currentCostume = s_rocket.costume;
    s_costumeScale = 1.0f;
}
