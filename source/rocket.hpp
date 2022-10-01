#pragma once

#define SDL_MAIN_HANDLED

#include <stdint.h>

#include <vector>
#include <map>
#include <random>

#include "define.hpp"
#include "cs_math.hpp"
using namespace csm;
#include "utility.hpp"
#include "memory.hpp"
#include "input.hpp"
#include "assets.hpp"

#include "cs_application.hpp"
#include "cs_audio.hpp"
#include "cs_graphics.hpp"
#include "cs_platform.hpp"

#include "cs_audio.cpp"
#include "cs_graphics.cpp"
#include "cs_platform.cpp"

#include <iomanip>
#include <stack>
#include <sstream>

using namespace cs;

enum GameState
{
    GameState_MainMenu,
    GameState_ScoresMenu,
    GameState_CostumesMenu,
    GameState_SettingsMenu,
    GameState_Game,
    GameState_TOTAL
};


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

static GameState s_gameState;
static u64       s_gameFrame;
static bool      s_gamePaused;
static bool      s_gameResetting;

#include "utility.cpp"
#include "memory.cpp"
#include "input.cpp"
#include "assets.cpp"
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
