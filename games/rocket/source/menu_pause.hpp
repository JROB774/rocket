#pragma once

enum PauseMenuOption
{
    PauseMenuOption_Resume,
    PauseMenuOption_Settings,
    PauseMenuOption_Menu,
    PauseMenuOption_TOTAL
};

static void PauseGame();
static void ResumeGame();

static void UpdatePauseMenu(f32 dt);
static void RenderPauseMenu(f32 dt);
static void GoToPauseMenu();
