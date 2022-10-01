#include "rocket.hpp"

static bool s_lockMouse = true;
static bool s_showMouse = false;

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

        auto& textures = GetAllAssetsOfType<Texture>();
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

        // PlayMusic("music", -1); // @INCOMPLETE: Add in music!

        s_gameState = GameState_MainMenu;
        s_gamePaused = false;
    }

    void OnQuit() override
    {
        SaveGame();
    }

    void OnUpdate(f32 dt) override
    {
        // Handle locking/unlocking and showing/hiding the mouse with debug mode.
        s_lockMouse = (!s_gamePaused && !s_rocket.dead && (s_gameState == GameState_Game));
        s_showMouse = false;

        LockMouse(s_lockMouse);
        ShowCursor(s_showMouse);

        switch(s_gameState)
        {
            case(GameState_MainMenu): UpdateMainMenu(dt); break;
            case(GameState_ScoresMenu): UpdateScoresMenu(dt); break;
            case(GameState_CostumesMenu): UpdateCostumesMenu(dt); break;
            case(GameState_SettingsMenu): UpdateSettingsMenu(dt); break;
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
    }
};

AppConfig AppMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.size = Vec2i(360,640);
    appConfig.window.min  = Vec2i(180,320);
    appConfig.screenSize  = Vec2i(180,320);
    appConfig.app = Allocate<RocketApp>(MEM_GAME);
    return appConfig;
}
