#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_utility.hpp"

//
// Collision
//

struct Collider
{
    cs::Vec2 offset;
    cs::f32 radius;
};

static bool CheckCollision(cs::Vec2 aPos, const Collider& a, cs::Vec2 bPos, const Collider& b)
{
    cs::f32 ax = aPos.x + a.offset.x, ay = aPos.y + a.offset.y;
    cs::f32 bx = bPos.x + b.offset.x, by = bPos.y + b.offset.y;
    cs::f32 x = abs(bx-ax);
    cs::f32 y = abs(by-ay);
    cs::f32 radius = a.radius+b.radius;
    return (((x*x)+(y*y)) <= (radius*radius));
}

//
// Asteroid
//

enum AsteroidType
{
    AsteroidType_Large,
    AsteroidType_Medium,
    AsteroidType_Small,
    AsteroidType_TOTAL
};

struct Asteroid
{
    cs::Vec2 pos;
    cs::imm::Flip flip;
    Collider collider;
    AsteroidType type;
};

static constexpr cs::f32 k_asteroidFallSpeed = 400.0f;
static constexpr cs::f32 k_asteroidMinSpinSpeed = 240.0f;
static constexpr cs::f32 k_asteroidMaxSpinSpeed = 420.0f;

static std::vector<Asteroid> s_asteroids;

static void SpawnAsteroid()
{
    Asteroid asteroid = {};
    asteroid.pos = cs::Vec2(cs::RandomF32(0, cs::gfx::GetScreenWidth()), -48.0f);
    asteroid.flip = (cs::RandomS32() % 2 == 0) ? cs::imm::Flip_None : cs::imm::Flip_Horizontal;
    asteroid.type = CS_CAST(AsteroidType, cs::RandomS32(0,AsteroidType_TOTAL));
    asteroid.collider.offset = cs::Vec2(0,-2);
    switch(asteroid.type)
    {
        case(AsteroidType_Large): asteroid.collider.radius = 12.0f; break;
        case(AsteroidType_Medium): asteroid.collider.radius = 8.0f; break;
        case(AsteroidType_Small): asteroid.collider.radius = 4.0f; break;
    }
    s_asteroids.push_back(asteroid);
}

static void UpdateAsteroids(cs::f32 dt)
{
    for(auto& asteroid: s_asteroids)
        asteroid.pos.y += k_asteroidFallSpeed * dt;

    s_asteroids.erase(std::remove_if(s_asteroids.begin(), s_asteroids.end(),
    [](const Asteroid& asteroid)
    {
        return (asteroid.pos.y >= (cs::gfx::GetScreenHeight()+48.0f));
    }),
    s_asteroids.end());
}

static void RenderAsteroids(cs::f32 dt)
{
    for(auto& asteroid: s_asteroids)
    {
        cs::Rect clip = { 0, CS_CAST(cs::f32, 48*asteroid.type), 48, 48 };
        cs::imm::DrawTexture("asteroid", asteroid.pos.x, asteroid.pos.y, 1.0f, 1.0f, 0.0f, asteroid.flip, &clip);
    }
}

static void DebugRenderAsteroids(cs::f32 dt)
{
    for(auto& asteroid: s_asteroids)
    {
        cs::Vec2 pos(asteroid.pos + asteroid.collider.offset);
        cs::imm::DrawCircleFilled(pos.x, pos.y, asteroid.collider.radius, cs::Vec4(1,0,0,0.25f));
        cs::imm::DrawCircleOutline(pos.x, pos.y, asteroid.collider.radius, cs::Vec4(1,0,0,1.00f));
    }
}

//
// Entity Spawn
//

enum EntityType
{
    EntityType_Asteroid,
    EntityType_TOTAL
};

static void MaybeSpawnEntity(cs::f32 dt)
{
    if(cs::RandomS32(0,1000) > 75) return;

    EntityType type = CS_CAST(EntityType, cs::RandomS32(0,EntityType_TOTAL-1));
    switch(type)
    {
        case(EntityType_Asteroid): SpawnAsteroid(); break;
    }
}

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

static void CreateSmoke()
{
    s_smoke.reserve(1024);
}

static void SpawnSmoke(cs::f32 x, cs::f32 y)
{
    Smoke s = {};
    s.pos = cs::Vec2(x,y);
    s.frame = 0;
    s.angle = cs::RandomF32(0,360.0f);
    s.timer = 0.0f;
    s.frameTime = cs::RandomF32(0.05f, 0.15f);
    s.dead = false;
    s_smoke.push_back(s);
}

static void UpdateSmoke(cs::f32 dt)
{
    for(auto& s: s_smoke)
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

    s_smoke.erase(std::remove_if(s_smoke.begin(), s_smoke.end(),
    [](const Smoke& s)
    {
        return s.dead;
    }),
    s_smoke.end());
}

static void RenderSmoke(cs::f32 dt)
{
    for(auto& s: s_smoke)
    {
        cs::Rect clip = { CS_CAST(cs::f32, 16*s.frame), 0, 16, 16 };
        cs::imm::DrawTexture("smoke", s.pos.x, s.pos.y, 1.0f, 1.0f, cs::ToRad(s.angle), cs::imm::Flip_None, &clip);
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
    Collider collider;
};

static Rocket s_rocket;

static void CreateRocket()
{
    s_rocket.pos.x = (cs::gfx::GetScreenWidth()/2.0f);
    s_rocket.pos.y = cs::gfx::GetScreenHeight() - 48.0f;
    s_rocket.vel   = cs::Vec2(0);
    s_rocket.angle = 0.0f;
    s_rocket.shake = 0.0f;
    s_rocket.timer = 0.0f;
    s_rocket.collider = { cs::Vec2(0,-8), 14.0f };
}

static void UpdateRocket(cs::f32 dt)
{
    if(cs::IsMouseLocked())
    {
        s_rocket.vel.x += cs::GetRelativeMousePos().x / 10.0f;
        s_rocket.vel.y += cs::GetRelativeMousePos().y / 20.0f;
    }

    s_rocket.angle = cs::Clamp(s_rocket.vel.x, -k_rocketMaxAngle, k_rocketMaxAngle);
    s_rocket.shake = cs::RandomF32(-k_rocketMaxShake, k_rocketMaxShake);

    s_rocket.vel.x = cs::Clamp(s_rocket.vel.x, -(k_rocketTerminalVelocity*1.5f), (k_rocketTerminalVelocity*1.5f));
    s_rocket.vel.y = cs::Clamp(s_rocket.vel.y, -k_rocketTerminalVelocity, k_rocketTerminalVelocity);

    s_rocket.pos += (s_rocket.vel * k_rocketVelocityMultiplier) * dt;
    s_rocket.pos.x = cs::Clamp(s_rocket.pos.x, 0.0f, cs::gfx::GetScreenWidth());
    s_rocket.pos.y = cs::Clamp(s_rocket.pos.y, 0.0f, cs::gfx::GetScreenHeight());

    s_rocket.vel = cs::Lerp(s_rocket.vel, cs::Vec2(0), cs::Vec2(0.1f));

    s_rocket.timer += dt;
    if(s_rocket.timer >= 0.05f)
    {
        SpawnSmoke(s_rocket.pos.x+cs::RandomF32(-3.0f,3.0f), s_rocket.pos.y+20.0f);
        s_rocket.timer -= 0.05f;
    }
}

static void RenderRocket(cs::f32 dt)
{
    static cs::Rect s_clip = { 48, 0, 48, 96 };
    cs::f32 angle = cs::ToRad(s_rocket.angle + s_rocket.shake);
    cs::imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, cs::imm::Flip_None, &s_clip);
    s_clip.x += 48.0f;
    if(s_clip.x >= 288.0f) s_clip.x = 48.0f;
}

static void DebugRenderRocket(cs::f32 dt)
{
    cs::Vec4 fill(0,1,0,0.25f);
    cs::Vec4 outline(0,1,0,1.00f);
    for(auto& asteroid: s_asteroids)
    {
        if(CheckCollision(s_rocket.pos, s_rocket.collider, asteroid.pos, asteroid.collider))
        {
            fill = cs::Vec4(1,0,0,0.25f);
            outline = cs::Vec4(1,0,0,1.00f);
        }
    }

    cs::Vec2 pos(s_rocket.pos + s_rocket.collider.offset);
    cs::imm::DrawCircleFilled(pos.x, pos.y, s_rocket.collider.radius, fill);
    cs::imm::DrawCircleOutline(pos.x, pos.y, s_rocket.collider.radius, outline);
}

//
// Background
//

static constexpr cs::s32 k_backCount = 3;

static cs::f32 s_backSpeed[k_backCount];
static cs::f32 s_backOffset[k_backCount];

static void CreateBackground()
{
    cs::f32 speed = 360.0f;
    for(cs::s32 i=k_backCount-1; i>=0; --i)
    {
        s_backSpeed[i] = speed;
        s_backOffset[i] = cs::gfx::GetScreenHeight() * 0.5f;
        speed += 12.0f;
    }
}

static void RenderBackground(cs::f32 dt)
{
    cs::gfx::Clear(0.0f, 0.05f, 0.2f);
    cs::f32 screenWidth = cs::gfx::GetScreenWidth();
    cs::f32 screenHeight = cs::gfx::GetScreenHeight();
    cs::Rect clip = { 0, 0, 180, 320 };
    cs::Vec4 color = cs::Vec4(1,1,1,0.4f);
    for(cs::s32 i=0; i<k_backCount; ++i)
    {
        s_backOffset[i] += s_backSpeed[i] * dt;
        cs::imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i], &clip, color);
        cs::imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i]-screenHeight, &clip, color);
        if(s_backOffset[i] >= screenHeight * 1.5f)
            s_backOffset[i] -= screenHeight;
        clip.x += 180.0f;
    }
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

        cs::gfx::SetTextureFilter(*cs::GetAsset<cs::gfx::Texture>("asteroid"), cs::gfx::Filter_Nearest);
        cs::gfx::SetTextureFilter(*cs::GetAsset<cs::gfx::Texture>("rocket"), cs::gfx::Filter_Nearest);
        cs::gfx::SetTextureFilter(*cs::GetAsset<cs::gfx::Texture>("smoke"), cs::gfx::Filter_Nearest);

        CreateBackground();
        CreateRocket();
        CreateSmoke();
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(cs::f32 dt)
    {
        // Handle locking/unlocking the mouse with debug mode.
        static bool s_lockMouse = true;
        if(cs::IsDebugMode())
        {
            if(cs::IsKeyPressed(cs::KeyCode_Escape))
                s_lockMouse = !s_lockMouse;
        }
        else
        {
            s_lockMouse = true;
        }
        cs::LockMouse(s_lockMouse);

        MaybeSpawnEntity(dt);
        UpdateAsteroids(dt);
        UpdateSmoke(dt);
        UpdateRocket(dt);
    }

    void Render(cs::f32 dt)
    {
        RenderBackground(dt);
        RenderSmoke(dt);
        RenderAsteroids(dt);
        RenderRocket(dt);
    }

    void DebugRender(cs::f32 dt)
    {
        DebugRenderAsteroids(dt);
        DebugRenderRocket(dt);
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
