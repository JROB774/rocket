#define SDL_MAIN_HANDLED

#define GLEW_STATIC

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <stdint.h>

#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <map>
#include <stack>
#include <random>
#include <iomanip>

#include <SDL.h>
#include <SDL_mixer.h>

#include <glew.h>
#include <glew.c>

#include <stb_image.h>

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
static size_t s_gameFrame;
static bool s_gamePaused;
static bool s_gameResetting;

#include "define.hpp"
#include "cs_math.hpp" // @Incomplete!
using namespace csm; // @Incomplete!
#include "utility.hpp"
#include "application.hpp"
#include "memory.hpp"
#include "input.hpp"
#include "assets.hpp"
#include "audio.hpp"
#include "graphics.hpp"
#include "platform.hpp"
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
#include "memory.cpp"
#include "input.cpp"
#include "assets.cpp"
#include "audio.cpp"
#include "graphics.cpp"
#include "platform.cpp"
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
