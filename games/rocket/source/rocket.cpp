#include "rocket.hpp"

static bool s_lockMouse = true;
static bool s_showMouse = false;

static void GameStateDebugUiCallback(bool& open)
{
    ImGui::Text("Lock Mouse: %s", (s_lockMouse) ? "True" : "False");
    ImGui::Text("Show Mouse: %s", (s_showMouse) ? "True" : "False");
    ImGui::Separator();
    ImGui::Text("Particle Count: %d", s_smoke.size());
    ImGui::Text("Asteroid Count: %d", s_asteroids.size());
    ImGui::Separator();
    ImGui::Text("Rocket");
    ImGui::Spacing();
    ImGui::Text("Position: (%f,%f)", s_rocket.pos.x,s_rocket.pos.y);
    ImGui::Text("Velocity: (%f,%f)", s_rocket.vel.x,s_rocket.vel.y);
    ImGui::Text("Angle: %f", s_rocket.angle);
    ImGui::Text("Shake: %f", s_rocket.shake);
    ImGui::Text("Timer: %f", s_rocket.timer);
    ImGui::Text("Score: %d", s_rocket.score);
    ImGui::Text("Frame: %d", s_rocket.frame);
    ImGui::Text("Dead: %s", (s_rocket.dead) ? "True" : "False");
    ImGui::Separator();
    ImGui::Text("Difficulty: %d", s_difficulty);
}

class RocketApp: public Application
{
public:
    void Init()
    {
        RegisterDebugUiWindow("Game State", GameStateDebugUiCallback);

        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);

        sfx::SetSoundVolume(0.4f);
        sfx::SetMusicVolume(0.0f);

        LoadAllAssetsOfType<gfx::Texture>();
        LoadAllAssetsOfType<gfx::Shader>();
        LoadAllAssetsOfType<sfx::Sound>();
        LoadAllAssetsOfType<sfx::Music>();

        auto& textures = GetAllAssetsOfType<gfx::Texture>();
        for(auto& texture: textures)
        {
            gfx::SetTextureFilter(*texture, gfx::Filter_Nearest);
            gfx::SetTextureWrap(*texture, gfx::Wrap_Clamp);
        }

        ShowCursor(false);

        CreateBackground();
        CreateRocket();
        CreateSmoke();

        LoadBitmapFont(   s_font0, 14,24,    "font0");
        LoadBitmapFont(   s_font1, 14,24,    "font1");
        LoadBitmapFont(s_bigFont0, 24,40, "bigfont0");
        LoadBitmapFont(s_bigFont1, 24,40, "bigfont1");

        sfx::PlayMusic("music", -1);

        s_gameState = GameState_MainMenu;
        s_gamePaused = false;
    }

    void Quit()
    {
        SaveGame();
    }

    void Update(f32 dt)
    {
        // Handle locking/unlocking and showing/hiding the mouse with debug mode.
        s_lockMouse = (!s_gamePaused && !s_rocket.dead && (s_gameState == GameState_Game));
        s_showMouse = (IsDebugMode());
        if(IsDebugMode() && IsKeyDown(KeyCode_LeftAlt))
            s_lockMouse = false;
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

    void Render(f32 dt)
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

    void DebugRender(f32 dt)
    {
        DebugRenderAsteroids(dt);
        DebugRenderRocket(dt);
    }
};

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.size = Vec2i(360,640);
    appConfig.window.min  = Vec2i(180,320);
    appConfig.screenSize  = Vec2i(180,320);
    appConfig.app = Allocate<RocketApp>(CS_MEM_GAME);
    return appConfig;
}
