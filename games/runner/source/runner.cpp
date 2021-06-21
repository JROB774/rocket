#include "runner.hpp"

class RunnerApp: public Application
{
public:
    void Init()
    {
        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(f32 dt)
    {
        // Nothing...
    }

    void Render(f32 dt)
    {
        gfx::Clear(1,0,1);
    }

    void DebugRender(f32 dt)
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
