#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_utility.hpp"

//
// Smoke
//

struct Smoke
{
    cs::Vec2 pos;
    cs::s32 frame;
    cs::f32 angle;
    cs::f32 timer;
    cs::f32 frameTime;
    bool dead;
};

static std::vector<Smoke> s_smoke;

static void SpawnSmoke(std::vector<Smoke>& smoke, cs::f32 x, cs::f32 y)
{
    Smoke s = {};
    s.pos = cs::Vec2(x,y);
    s.frame = 0;
    s.angle = cs::ToRad(cs::RandomF32(0,360.0f));
    s.timer = 0.0f;
    s.frameTime = cs::RandomF32(0.05f, 0.15f);
    s.dead = false;
    smoke.push_back(s);
}

static void UpdateSmoke(std::vector<Smoke>& smoke, cs::f32 dt)
{
    for(auto& s: smoke)
    {
        s.pos.y += 180.0f * dt;
        s.timer += dt;
        if(s.timer >= s.frameTime)
        {
            s.frame++;
            s.timer = 0.0f;
            s.frameTime = cs::RandomF32(0.05f, 0.15f);
        }
        if(s.frame >= 8)
            s.dead = true;
    }

    smoke.erase(std::remove_if(smoke.begin(), smoke.end(),
    [](const Smoke& s)
    {
        return s.dead;
    }),
    smoke.end());
}

static void RenderSmoke(const std::vector<Smoke>& smoke, cs::f32 dt)
{
    for(auto& s: smoke)
    {
        cs::Rect clip = { CS_CAST(cs::f32, 16*s.frame), 0, 16, 16 };
        cs::imm::DrawTexture("smoke", s.pos.x, s.pos.y, 1.0f, 1.0f, s.angle, cs::imm::Flip_None, &clip);
    }
}

//
// Rocket
//

static constexpr cs::f32 k_rocketVelocityMultiplier = 25.0f;
static constexpr cs::f32 k_rocketTerminalVelocity = 9.5;
static constexpr cs::f32 k_rocketMaxAngle = 25.0f;
static constexpr cs::f32 k_rocketMaxShake = 2.0f;

struct Rocket
{
    cs::Vec2 pos;
    cs::Vec2 vel;
    cs::f32 angle;
    cs::f32 shake;
    cs::f32 timer;
};

static Rocket s_rocket;

static void CreateRocket(Rocket& rocket)
{
    rocket.pos.x = (cs::gfx::GetScreenWidth()/2.0f);
    rocket.pos.y = cs::gfx::GetScreenHeight() - 48.0f;
    rocket.vel   = cs::Vec2(0);
    rocket.angle = 0.0f;
    rocket.shake = 0.0f;
    rocket.timer = 0.0f;
}

static void UpdateRocket(Rocket& rocket, cs::f32 dt)
{
    rocket.vel.x += cs::GetRelativeMousePos().x / 10.0f;
    rocket.vel.y += cs::GetRelativeMousePos().y / 20.0f;

    rocket.angle = cs::Clamp(rocket.vel.x, -k_rocketMaxAngle, k_rocketMaxAngle);
    rocket.shake = cs::RandomF32(-k_rocketMaxShake, k_rocketMaxShake);

    rocket.vel.x = cs::Clamp(rocket.vel.x, -(k_rocketTerminalVelocity*1.5f), (k_rocketTerminalVelocity*1.5f));
    rocket.vel.y = cs::Clamp(rocket.vel.y, -k_rocketTerminalVelocity, k_rocketTerminalVelocity);

    rocket.pos += (rocket.vel * k_rocketVelocityMultiplier) * dt;
    rocket.pos.x = cs::Clamp(rocket.pos.x, 0.0f, cs::gfx::GetScreenWidth());
    rocket.pos.y = cs::Clamp(rocket.pos.y, 0.0f, cs::gfx::GetScreenHeight());

    rocket.vel = cs::Lerp(rocket.vel, cs::Vec2(0), cs::Vec2(0.1f));

    rocket.timer += dt;
    if(rocket.timer >= 0.05f)
    {
        SpawnSmoke(s_smoke, rocket.pos.x+cs::RandomF32(-3.0f,3.0f), rocket.pos.y+20.0f);
        rocket.timer -= 0.05f;
    }
}

static void RenderRocket(Rocket& rocket, cs::f32 dt)
{
    static cs::Rect s_clip = { 48, 0, 48, 96 };
    cs::f32 angle = cs::ToRad(rocket.angle + rocket.shake);
    cs::imm::DrawTexture("rocket", rocket.pos.x, rocket.pos.y, 1.0f, 1.0f, angle, cs::imm::Flip_None, &s_clip);
    s_clip.x += 48.0f;
    if(s_clip.x >= 288.0f) s_clip.x = 48.0f;
}

//
// Application
//

class RocketApp: public cs::Application
{
public:
    void Init()
    {
        cs::gfx::SetScreenScaleMode(cs::gfx::ScaleMode_Pixel);
        cs::gfx::SetScreenFilter(cs::gfx::Filter_Nearest);

        cs::gfx::SetTextureFilter(*cs::GetAsset<cs::gfx::Texture>("rocket"), cs::gfx::Filter_Nearest);
        cs::gfx::SetTextureFilter(*cs::GetAsset<cs::gfx::Texture>("smoke"), cs::gfx::Filter_Nearest);

        CreateRocket(s_rocket);
        s_smoke.reserve(1024);
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(cs::f32 dt)
    {
        cs::LockMouse(!cs::IsDebugMode());
        UpdateSmoke(s_smoke, dt);
        UpdateRocket(s_rocket, dt);
    }

    void Render(cs::f32 dt)
    {
        cs::gfx::Clear(0.0f, 0.05f, 0.2f);
        RenderSmoke(s_smoke, dt);
        RenderRocket(s_rocket, dt);
    }

    void DebugRender(cs::f32 dt)
    {
        // Nothing...
    }
};

cs::AppConfig csMain(int argc, char** argv)
{
    cs::AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.min = cs::Vec2i(180,320);
    appConfig.screenSize = cs::Vec2i(180,320);
    appConfig.app = cs::Allocate<RocketApp>(CS_MEM_GAME);
    return appConfig;
}
