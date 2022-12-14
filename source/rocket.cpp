#define GLEW_STATIC

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#define NK_STATIC
#define NK_MATH_IMPLEMENTATION
#define NK_FILESYS_IMPLEMENTATION
#define NK_PACK_IMPLEMENTATION

#define NOMINMAX

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

#include <stb_image.h>

#include <nk_define.h>
#include <nk_defer.h>
#include <nk_math.h>
#include <nk_mathx.h>
#include <nk_pack.h>

#ifndef __EMSCRIPTEN__
#include <glew.c>
#include <gon.cpp>
#else
#include <GLES2/gl2.h>
#include <emscripten.h>
#endif

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
static bool s_gameUnfocused;
static bool s_gameResetting;

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
#include "cursor.hpp"
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
#include "cursor.cpp"
#include "menu.cpp"
#include "menu_main.cpp"
#include "menu_scores.cpp"
#include "menu_costumes.cpp"
#include "menu_settings.cpp"
#include "menu_gameover.cpp"
#include "menu_pause.cpp"

class RocketApp: public Application
{
public:
    void OnInit() override
    {
        SetScreenScaleMode(ScaleMode_Pixel);
        SetScreenFilter(Filter_Nearest);

        LoadAllAssetsOfType<Texture>();
        LoadAllAssetsOfType<Shader>();
        LoadAllAssetsOfType<Sound>();
        LoadAllAssetsOfType<Music>();

        auto textures = GetAllAssetsOfType<Texture>();
        for(auto& texture: textures)
        {
            SetTextureFilter(*texture, Filter_Nearest);
            SetTextureWrap(*texture, Wrap_Clamp);
        }

        ShowCursor(false);

        CreateBackground();
        CreateRocket();
        CreateSmoke();

        LoadBitmapFont(   s_font0, 14,24,    "font0");
        LoadBitmapFont(   s_font1, 14,24,    "font1");
        LoadBitmapFont(s_bigFont0, 24,40, "bigfont0");
        LoadBitmapFont(s_bigFont1, 24,40, "bigfont1");

        // PlayMusic("music", -1); // @Incomplete: Add in music!

        s_gameState = GameState_MainMenu;
        s_gamePaused = false;
    }

    void OnQuit() override
    {
        SaveGame();
    }

    void OnUpdate(f32 dt) override
    {
        UpdateCursor(dt);

        if(!s_gameUnfocused)
        {
            switch(s_gameState)
            {
                case GameState_MainMenu: UpdateMainMenu(dt); break;
                case GameState_ScoresMenu: UpdateScoresMenu(dt); break;
                case GameState_CostumesMenu: UpdateCostumesMenu(dt); break;
                case GameState_SettingsMenu: UpdateSettingsMenu(dt); break;
                default:
                {
                    // Nothing...
                } break;
            }

            UpdateGameOverMenu(dt);
            UpdatePauseMenu(dt);

            if(!s_gamePaused)
            {
                if((s_gameState == GameState_Game) && !s_gameResetting)
                    MaybeSpawnEntity(dt);
                UpdateBackground(dt);
                UpdateAsteroids(dt);
                UpdateSmoke(dt);
                UpdateRocket(dt);
            }
        }

        s_gameFrame++;
    }

    void OnRender(f32 dt) override
    {
        RenderBackground(dt);
        RenderSmoke(dt);
        RenderAsteroids(dt);
        RenderRocket(dt);
        RenderPauseMenu(dt);
        RenderMainMenu(dt);
        RenderScoresMenu(dt);
        RenderCostumesMenu(dt);
        RenderSettingsMenu(dt);
        RenderGameOverMenu(dt);
        RenderCursor(dt);
        RenderTransition(dt);
        RenderUnfocused(dt);
    }
};

AppConfig AppMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.size = { 360,640 };
    appConfig.window.min  = { 180,320 };
    appConfig.screenSize  = { 180,320 };
    appConfig.app = Allocate<RocketApp>(MEM_GAME);
    return appConfig;
}
