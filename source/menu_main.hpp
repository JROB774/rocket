#pragma once

enum MainMenuOptionID
{
    MainMenuOption_Start,
    MainMenuOption_Scores,
    MainMenuOption_Costumes,
    MainMenuOption_Settings,
    #ifndef __EMSCRIPTEN__
    MainMenuOption_Exit,
    #endif
    MainMenuOption_TOTAL
};

static void UpdateMainMenu(f32 dt);
static void RenderMainMenu(f32 dt);
static void GoToMainMenu();
