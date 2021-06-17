#pragma once

#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_audio.hpp"
#include "cs_utility.hpp"

#include <iomanip>
#include <stack>
#include <sstream>

using namespace cs;

static constexpr f32 k_fallSpeed = 400.0f;

enum GameState
{
    GameState_MainMenu,
    GameState_ScoresMenu,
    GameState_CostumesMenu,
    GameState_SettingsMenu,
    GameState_Game,
    GameState_TOTAL
};

static GameState s_gameState;
static bool s_gamePaused;
static bool s_gameResetting;
static u32 s_gameFrame;

#include "utility.hpp"
#include "collision.hpp"
#include "bitmap_font.hpp"
#include "save.hpp"
#include "asteroid.hpp"
#include "smoke.hpp"
#include "player.hpp"
#include "transition.hpp"
#include "background.hpp"
#include "menu.hpp"
#include "menu_main.hpp"
#include "menu_scores.hpp"
#include "menu_costumes.hpp"
#include "menu_settings.hpp"
#include "menu_gameover.hpp"
#include "menu_pause.hpp"

#include "utility.cpp"
#include "collision.cpp"
#include "bitmap_font.cpp"
#include "save.cpp"
#include "asteroid.cpp"
#include "smoke.cpp"
#include "player.cpp"
#include "transition.cpp"
#include "background.cpp"
#include "menu.cpp"
#include "menu_main.cpp"
#include "menu_scores.cpp"
#include "menu_costumes.cpp"
#include "menu_settings.cpp"
#include "menu_gameover.cpp"
#include "menu_pause.cpp"
