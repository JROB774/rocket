#pragma once

enum ScoresMenuOption
{
    ScoresMenuOption_Back,
    ScoresMenuOption_TOTAL
};

static void UpdateScoresMenu(f32 dt);
static void RenderScoresMenu(f32 dt);
static void GoToScoresMenu();
