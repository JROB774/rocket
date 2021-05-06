#include "hungry.hpp"

class HungryApp: public Application
{
public:
    void Init() override
    {
        // Nothing...
    }

    void Quit() override
    {
        // Nothing...
    }

    void Update(f32 dt) override
    {
        // Nothing...
    }

    void Render(f32 dt) override
    {
        gfx::Clear(1,0,0);
    }
};

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Hungry";
    appConfig.window.min = Vec2i(320,180);
    appConfig.app = Allocate<HungryApp>(CS_MEM_GAME);
    return appConfig;
}
