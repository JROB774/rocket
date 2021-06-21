#include "runner.hpp"

class RunnerApp: public Application
{
public:
    void OnInit() override
    {
        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);

        LoadAllAssetsOfType<gfx::Texture>();
        LoadAllAssetsOfType<gfx::Shader>();
        LoadAllAssetsOfType<sfx::Sound>();
        LoadAllAssetsOfType<sfx::Music>();
        LoadAllAssetsOfType<Animation>();
    }

    void OnQuit() override
    {
        // Nothing...
    }

    void OnUpdate(f32 dt) override
    {
        // Nothing...
    }

    void OnRender(f32 dt) override
    {
        f32 halfWidth = gfx::GetScreenWidth() * 0.5f;
        f32 halfHeight = gfx::GetScreenHeight() * 0.5f;

        imm::DrawTexture("bg", halfWidth,halfHeight);
        imm::DrawTexture("fg", halfWidth,halfHeight);
    }

    void OnDebugRender(f32 dt) override
    {
        // Nothing...
    }
};

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Runner";
    appConfig.window.size = Vec2i(480,288);
    appConfig.window.min  = Vec2i(240,144);
    appConfig.screenSize  = Vec2i(240,144);
    appConfig.app = Allocate<RunnerApp>(CS_MEM_GAME);
    return appConfig;
}
