#pragma once

enum GameOverMenuOption
{
    GameOverMenuOption_Retry,
    GameOverMenuOption_Menu,
    GameOverMenuOption_TOTAL
};

static std::vector<Costume> s_gameOverUnlocks;

static void UpdateGameOverMenu(f32 dt);
static void RenderGameOverMenu(f32 dt);
static void GoToGameOverMenu();
