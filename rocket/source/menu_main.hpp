#pragma once

enum MainMenuOptionID
{
    MainMenuOption_Start,
    MainMenuOption_Scores,
    MainMenuOption_Costumes,
    MainMenuOption_Settings,
    MainMenuOption_Exit,
    MainMenuOption_TOTAL
};

static void UpdateMainMenu(f32 dt);
static void RenderMainMenu(f32 dt);
static void GoToMainMenu();
