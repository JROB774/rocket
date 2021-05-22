#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_audio.hpp"
#include "cs_utility.hpp"

#include <iomanip>
#include <sstream>

using namespace cs;

//
// Game
//

enum GameState
{
    GameState_Menu,
    GameState_Game
};

static GameState s_gameState;

//
// Utility
//

static f32 SinRange(f32 min, f32 max, f32 t)
{
    f32 halfRange = (max - min) / 2;
    return (min + halfRange + sinf(t) * halfRange);
}

static Vec2 RotateVec2(Vec2 vec, f32 rad)
{
    f32 x = vec.x * cosf(rad) - vec.y * sinf(rad);
    f32 y = vec.x * sinf(rad) + vec.y * cosf(rad);
    return Vec2(x,y);
}

//
// Bitmap Font
//

struct BitmapFont
{
    Rect bounds[256];
    std::string texture;
    f32 charWidth;
    f32 charHeight;
};

static BitmapFont s_font;

static void LoadBitmapFont(BitmapFont& font, f32 cw, f32 ch, std::string texture)
{
    font.texture = texture;
    font.charWidth = cw;
    font.charHeight = ch;

    for(s32 iy=0; iy<3; ++iy)
        for(s32 ix=0; ix<32; ++ix)
            font.bounds[iy*32+ix] = { CS_CAST(f32,ix)*cw, CS_CAST(f32,iy)*ch, cw, ch };
}

static f32 GetTextLineWidth(BitmapFont& font, std::string text, s32 line = 0)
{
    f32 lineWidth = 0;
    s32 lineIndex = 0;
    for(size_t i=0; i<text.length(); ++i)
    {
        if(text[i] == '\n')
        {
            if(lineIndex++ == line) return lineWidth;
            else lineWidth = 0;
        }
        else
        {
            lineWidth += font.charWidth;
        }
    }
    return lineWidth;
}

static void DrawBitmapFont(BitmapFont& font, f32 x, f32 y, std::string text, Vec4 color = Vec4(1))
{
    // NOTE: We just assume the caller wants multi-line text to be center aligned.

    f32 ix = x;
    f32 iy = y;

    s32 line = 0;

    for(size_t i=0; i<text.length(); ++i)
    {
        if(text[i] == '\n')
        {
            ix = x + (GetTextLineWidth(font, text, 0)*0.5f) - (GetTextLineWidth(font, text, line+1)*0.5f);
            iy += font.charHeight;
            line++;
        }
        else
        {
            f32 dx = ix + (font.charWidth*0.5f);
            f32 dy = iy + (font.charHeight*0.5f);
            Rect bounds = font.bounds[CS_CAST(u8,text.at(i))];
            imm::DrawTexture(font.texture, dx,dy, &bounds);
            ix += font.charWidth;
        }
    }
}

//
// Collision
//

struct Collider
{
    Vec2 offset;
    f32 radius;
};

static bool CheckCollision(Vec2 aPos, const Collider& a, Vec2 bPos, const Collider& b)
{
    f32 ax = aPos.x + a.offset.x, ay = aPos.y + a.offset.y;
    f32 bx = bPos.x + b.offset.x, by = bPos.y + b.offset.y;
    f32 x = abs(bx-ax);
    f32 y = abs(by-ay);
    f32 radius = a.radius+b.radius;
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
    Vec2 pos;
    imm::Flip flip;
    Collider collider;
    AsteroidType type;
};

static constexpr f32 k_asteroidFallSpeed = 400.0f;
static constexpr f32 k_asteroidMinSpinSpeed = 240.0f;
static constexpr f32 k_asteroidMaxSpinSpeed = 420.0f;

static std::vector<Asteroid> s_asteroids;

static void SpawnAsteroid()
{
    Asteroid asteroid = {};
    asteroid.pos = Vec2(RandomF32(0, gfx::GetScreenWidth()), -48.0f);
    asteroid.flip = (RandomS32() % 2 == 0) ? imm::Flip_None : imm::Flip_Horizontal;
    asteroid.type = CS_CAST(AsteroidType, RandomS32(0,AsteroidType_TOTAL-1));
    asteroid.collider.offset = Vec2(0,-2);
    switch(asteroid.type)
    {
        case(AsteroidType_Large): asteroid.collider.radius = 12.0f; break;
        case(AsteroidType_Medium): asteroid.collider.radius = 8.0f; break;
        case(AsteroidType_Small): asteroid.collider.radius = 4.0f; break;
    }
    s_asteroids.push_back(asteroid);
}

static void UpdateAsteroids(f32 dt)
{
    for(auto& asteroid: s_asteroids)
        asteroid.pos.y += k_asteroidFallSpeed * dt;

    s_asteroids.erase(std::remove_if(s_asteroids.begin(), s_asteroids.end(),
    [](const Asteroid& asteroid)
    {
        return (asteroid.pos.y >= (gfx::GetScreenHeight()+48.0f));
    }),
    s_asteroids.end());
}

static void RenderAsteroids(f32 dt)
{
    for(auto& asteroid: s_asteroids)
    {
        Rect clip = { CS_CAST(f32, 48*asteroid.type), 0, 48, 48 };
        imm::DrawTexture("asteroid", asteroid.pos.x, asteroid.pos.y, 1.0f, 1.0f, 0.0f, asteroid.flip, &clip);
    }
}

static void DebugRenderAsteroids(f32 dt)
{
    for(auto& asteroid: s_asteroids)
    {
        Vec2 pos(asteroid.pos + asteroid.collider.offset);
        imm::DrawCircleFilled(pos.x, pos.y, asteroid.collider.radius, Vec4(1,0,0,0.25f));
        imm::DrawCircleOutline(pos.x, pos.y, asteroid.collider.radius, Vec4(1,0,0,1.00f));
    }
}

//
// Star
//

struct Star
{
    Vec2 pos;
    f32 speed;
    f32 angle;
    f32 spin;
    f32 timer;
    s32 frame;
    Collider collider;
};

static constexpr f32 k_starMoveSpeed = 180.0f;
static constexpr f32 k_starFallSpeed = 400.0f;
static constexpr f32 k_starSpinSpeed = 240.0f;

static std::vector<Star> s_stars;

static void SpawnStar()
{
    Star star = {};
    star.pos.x = (RandomS32() % 2 == 0) ? -32 : gfx::GetScreenWidth()+32.0f;
    star.pos.y = -32;
    star.speed = (star.pos.x < 0.0f) ? k_starMoveSpeed : -k_starMoveSpeed;
    star.angle = 0.0f;
    star.spin = (star.pos.x < 0.0f) ? k_starSpinSpeed : -k_starSpinSpeed;
    star.timer = 0.0f;
    star.frame = 0;
    star.collider = { Vec2(0), 8 };
    s_stars.push_back(star);
}

static void UpdateStars(f32 dt)
{
    for(auto& star: s_stars)
    {
        star.timer += dt;
        if(star.timer >= 0.05f)
        {
            star.frame++;
            star.timer -= 0.05f;
        }
        if(star.frame >= 13)
            star.frame = 0;

        star.angle += star.spin * dt;
        star.pos.x += star.speed * dt;
        star.pos.y += k_starFallSpeed * dt;
    }

    s_stars.erase(std::remove_if(s_stars.begin(), s_stars.end(),
    [](const Star& star)
    {
        return ((star.speed < 0) ? (star.pos.x <= -32.0f) : (star.pos.x >= gfx::GetScreenWidth()+32.0f));
    }),
    s_stars.end());
}

static void RenderStars(f32 dt)
{
    for(auto& star: s_stars)
    {
        // Trail.
        Rect clip = { 416, 0, 32, 32 };
        f32 alpha = 0.02f;
        f32 offset = (star.speed < 0) ? 5 : -5;
        for(s32 i=10; i>=0; --i)
        {
            imm::DrawTexture("star", star.pos.x+(offset*i), star.pos.y, 1.0f, 1.0f, csm::ToRad(star.angle), imm::Flip_None, &clip, Vec4(1,1,1,alpha));
            alpha += 0.02f;
        }
        // Star
        clip = { CS_CAST(f32, 32*star.frame), 0, 32, 32 };
        imm::DrawTexture("star", star.pos.x, star.pos.y, 1.0f, 1.0f, csm::ToRad(star.angle), imm::Flip_None, &clip);
    }
}

static void DebugRenderStars(f32 dt)
{
    for(auto& star: s_stars)
    {
        Vec2 pos(star.pos + star.collider.offset);
        imm::DrawCircleFilled(pos.x, pos.y, star.collider.radius, Vec4(1,0,0,0.25f));
        imm::DrawCircleOutline(pos.x, pos.y, star.collider.radius, Vec4(1,0,0,1.00f));
    }
}

//
// Entity Spawn
//

enum EntityType
{
    EntityType_Asteroid,
    EntityType_Star,
    EntityType_TOTAL
};

static f32 s_entitySpawnCooldown;

static void MaybeSpawnEntity(f32 dt)
{
    if(s_entitySpawnCooldown > 0.0f)
    {
        s_entitySpawnCooldown -= dt;
    }
    else
    {
        if(RandomS32(0,1000) <= 75)
            SpawnAsteroid();
    }
}

//
// Smoke
//

enum SmokeType
{
    SmokeType_Thruster,
    SmokeType_Explosion,
    SmokeType_Stationary
};

struct Smoke
{
    SmokeType type;
    Vec2 pos;
    Vec2 vel;
    s32 frame;
    f32 angle;
    f32 spin;
    f32 timer;
    f32 frameTime;
    bool spawner;
    bool dead;
};

static std::vector<Smoke> s_smoke;

static void CreateSmoke()
{
    s_smoke.reserve(1024);
}

static void SpawnSmoke(SmokeType type, f32 x, f32 y, s32 count)
{
    for(s32 i=0; i<count; ++i)
    {
        Smoke s = {};
        s.type = type;
        s.pos = Vec2(x,y);
        s.frame = 0;
        s.angle = RandomF32(0,360.0f);
        s.vel = RotateVec2(Vec2(RandomF32(80,140),0), csm::ToRad(s.angle));
        s.spin = RandomF32(400,600);
        s.timer = 0.0f;
        s.frameTime = RandomF32(0.05f, 0.15f);
        s.spawner = (RandomS32(1,100) <= 10);
        s.dead = false;
        if(s.spawner) s.vel = (s.vel * 3.0f) + RandomF32(0,40);
        s_smoke.push_back(s);
    }
}

static void UpdateSmoke(f32 dt)
{
    for(auto& s: s_smoke)
    {
        s.timer += dt;
        if(s.timer >= s.frameTime)
        {
            s.frame++;
            s.timer = 0.0f;
            s.frameTime = RandomF32(0.05f, 0.15f);
        }
        if(s.frame >= 8)
        {
            s.dead = true;
        }

        // Different smoke types move differently.
        switch(s.type)
        {
            case(SmokeType_Thruster):
            {
                s.pos.y += 180.0f * dt;
            } break;
            case(SmokeType_Explosion):
            {
                s.pos += s.vel * dt;
                s.angle += s.spin * dt;
                if(s.spawner)
                {
                    if(RandomS32(1,100) < 25)
                    {
                        SpawnSmoke(SmokeType_Stationary, s.pos.x, s.pos.y, 1);
                    }
                }
                else
                {
                    if(RandomS32(1,100) < 5)
                    {
                        SpawnSmoke(SmokeType_Stationary, s.pos.x, s.pos.y, 1);
                    }
                }
            } break;
        }
    }

    s_smoke.erase(std::remove_if(s_smoke.begin(), s_smoke.end(),
    [](const Smoke& s)
    {
        return s.dead;
    }),
    s_smoke.end());
}

static void RenderSmoke(f32 dt)
{
    for(auto& s: s_smoke)
    {
        Rect clip = { CS_CAST(f32, 16*s.frame), 0, 16, 16 };
        imm::DrawTexture("smoke", s.pos.x, s.pos.y, 1.0f, 1.0f, csm::ToRad(s.angle), imm::Flip_None, &clip);
    }
}

//
// Rocket
//

static constexpr f32 k_rocketVelocityMultiplier = 25.0f;
static constexpr f32 k_rocketTerminalVelocity = 9.5f;
static constexpr f32 k_rocketMaxAngle = 25.0f;
static constexpr f32 k_rocketMaxShake = 2.0f;

struct Rocket
{
    Vec2 pos;
    Vec2 vel;
    f32 angle;
    f32 shake;
    f32 timer;
    s32 score;
    bool dead;
    Collider collider;
    sfx::SoundRef thruster;
};

static Rocket s_rocket;

static void CreateRocket()
{
    s_rocket.pos.x = (gfx::GetScreenWidth()*0.5f);
    s_rocket.pos.y = gfx::GetScreenHeight() - 64.0f;
    s_rocket.vel   = Vec2(0);
    s_rocket.angle = 0.0f;
    s_rocket.shake = 0.0f;
    s_rocket.timer = 0.0f;
    s_rocket.score = 0;
    s_rocket.dead  = false;
    s_rocket.collider = { Vec2(0,-8), 8.0f };
    s_rocket.thruster = sfx::PlaySound("thruster", -1);
}

static void RespawnRocket()
{
    s_rocket.thruster = sfx::PlaySound("thruster", -1);
    s_rocket.dead = false;
    s_rocket.score = 0;
}

static void HitRocket()
{
    SpawnSmoke(SmokeType_Explosion, s_rocket.pos.x, s_rocket.pos.y, RandomS32(20,40));
    sfx::StopSound(s_rocket.thruster);
    sfx::PlaySound("explosion");
    s_rocket.dead = true;
    s_rocket.timer = 0.0f;
}

static void UpdateRocket(f32 dt)
{
    if(s_rocket.dead)
    {
        // ...
    }
    else
    {
        if(IsMouseLocked())
        {
            static f32 s_prevMouseX = 0.0f;
            static f32 s_currMouseX = 0.0f;

            s_prevMouseX = s_currMouseX;
            s_currMouseX = GetRelativeMousePos().x;

            s_rocket.vel.x += GetRelativeMousePos().x / 10.0f;
            s_rocket.vel.y += GetRelativeMousePos().y / 20.0f;

            // If the player moved the mouse fast enough then play a whoosh sound.
            static bool s_canPlayWhoosh = true;
            static f32 s_whooshVel = 0.0f;
            if(abs(s_currMouseX - s_prevMouseX) >= 35.0f)
            {
                if(s_canPlayWhoosh)
                {
                    sfx::PlaySound("whoosh");
                    s_whooshVel = s_rocket.vel.x;
                    s_canPlayWhoosh = false;
                }
            }
            if(!s_canPlayWhoosh)
            {
                // Direction change.
                if(s_whooshVel > 0.0f && s_rocket.vel.x < 0.0f || s_whooshVel < 0.0f && s_rocket.vel.x > 0.0f)
                    s_canPlayWhoosh = true;
                // Speed goes down.
                if(s_rocket.vel.x <= 5.0f && s_rocket.vel.x >= -5.0f)
                    s_canPlayWhoosh = true;
            }
        }

        s_rocket.angle = csm::Clamp(s_rocket.vel.x, -k_rocketMaxAngle, k_rocketMaxAngle);
        s_rocket.shake = RandomF32(-k_rocketMaxShake, k_rocketMaxShake);

        s_rocket.vel.x = csm::Clamp(s_rocket.vel.x, -(k_rocketTerminalVelocity*1.5f), (k_rocketTerminalVelocity*1.5f));
        s_rocket.vel.y = csm::Clamp(s_rocket.vel.y, -k_rocketTerminalVelocity, k_rocketTerminalVelocity);

        s_rocket.pos += (s_rocket.vel * k_rocketVelocityMultiplier) * dt;
        s_rocket.pos.x = csm::Clamp(s_rocket.pos.x, 0.0f, gfx::GetScreenWidth());
        s_rocket.pos.y = csm::Clamp(s_rocket.pos.y, 0.0f, gfx::GetScreenHeight());

        s_rocket.vel = csm::Lerp(s_rocket.vel, Vec2(0), Vec2(0.1f));

        s_rocket.timer += dt;
        if(s_rocket.timer >= 0.05f)
        {
            SpawnSmoke(SmokeType_Thruster, s_rocket.pos.x+RandomF32(-3.0f,3.0f), s_rocket.pos.y+20.0f, 1);
            s_rocket.timer -= 0.05f;
        }

        // Handle collision checks.
        if(s_gameState == GameState_Game)
        {
            for(auto& asteroid: s_asteroids)
                if(CheckCollision(s_rocket.pos, s_rocket.collider, asteroid.pos, asteroid.collider))
                    HitRocket();
        }

        // Increment the score.
        if(s_gameState == GameState_Game)
            ++s_rocket.score;
    }
}

static void RenderRocket(f32 dt)
{
    if(s_rocket.dead)
    {
        // Draw the explosion.
        s_rocket.timer += dt;
        f32 frame = floorf(s_rocket.timer / 0.04f);
        if(frame < 13)
        {
            Rect clip = { 96*frame, 0, 96, 96 };
            imm::DrawTexture("explosion", s_rocket.pos.x, s_rocket.pos.y, &clip);
            imm::DrawTexture("explosion", s_rocket.pos.x-20, s_rocket.pos.y-10, 0.5f,0.5f, 0.0f, imm::Flip_None,  &clip);
            imm::DrawTexture("explosion", s_rocket.pos.x+10, s_rocket.pos.y+30, 0.5f,0.5f, 0.0f, imm::Flip_None,  &clip);
        }
    }
    else
    {
        // Draw the rocket.
        static Rect s_clip = { 48, 0, 48, 96 };
        f32 angle = csm::ToRad(s_rocket.angle + s_rocket.shake);
        imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, imm::Flip_None, &s_clip);
        s_clip.x += 48.0f;
        if(s_clip.x >= 288.0f) s_clip.x = 48.0f;
    }

    // Draw the score.
    if(s_gameState == GameState_Game)
    {
        std::string scoreStr = std::to_string(s_rocket.score);
        f32 screenWidth = gfx::GetScreenWidth();
        f32 screenHeight = gfx::GetScreenHeight();
        f32 textWidth = GetTextLineWidth(s_font, scoreStr);
        DrawBitmapFont(s_font, (screenWidth-textWidth)*0.5f,0.0f, scoreStr);
    }
}

static void DebugRenderRocket(f32 dt)
{
    Vec4 fill(0,1,0,0.25f);
    Vec4 outline(0,1,0,1.00f);
    Vec2 pos(s_rocket.pos + s_rocket.collider.offset);
    imm::DrawCircleFilled(pos.x, pos.y, s_rocket.collider.radius, fill);
    imm::DrawCircleOutline(pos.x, pos.y, s_rocket.collider.radius, outline);
}

//
// Background
//

static constexpr s32 k_backCount = 3;

static f32 s_backSpeed[k_backCount];
static f32 s_backOffset[k_backCount];

static void CreateBackground()
{
    f32 speed = 360.0f;
    for(s32 i=k_backCount-1; i>=0; --i)
    {
        s_backSpeed[i] = speed;
        s_backOffset[i] = gfx::GetScreenHeight() * 0.5f;
        speed += 120.0f;
    }
}

static void RenderBackground(f32 dt)
{
    gfx::Clear(0.0f, 0.05f, 0.2f);
    f32 screenWidth = gfx::GetScreenWidth();
    f32 screenHeight = gfx::GetScreenHeight();
    Rect clip = { 0, 0, 180, 320 };
    Vec4 color = Vec4(1,1,1,0.4f);
    for(s32 i=0; i<k_backCount; ++i)
    {
        s_backOffset[i] += s_backSpeed[i] * dt;
        imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i], &clip, color);
        imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i]-screenHeight, &clip, color);
        if(s_backOffset[i] >= screenHeight * 1.5f)
            s_backOffset[i] -= screenHeight;
        clip.x += 180.0f;
    }
}

//
// Menu
//

static void UpdateMenu(f32 dt)
{
    if(s_gameState == GameState_Menu)
    {
        if(IsMouseButtonPressed(MouseButton_Left))
        {
            s_gameState = GameState_Game;
            s_entitySpawnCooldown = 1.0f;
        }
    }
}

static void RenderMenu(f32 dt)
{
    static f32 s_targetScaleX = 0.0f;
    static f32 s_targetScaleY = 10.0f;

    static f32 s_scaleX = 1.0f;
    static f32 s_scaleY = 1.0f;
    static f32 s_angle  = 0.0f;
    static f32 s_timer  = 0.0f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    if(s_gameState == GameState_Menu)
    {
        s_scaleX = SinRange(0.8f, 1.0f, s_timer*1.5f);
        s_scaleY = SinRange(0.8f, 1.0f, s_timer*2.0f);
    }
    else
    {
        s_scaleX = csm::Lerp(s_scaleX, s_targetScaleX, dt*30.0f);
        s_scaleY = csm::Lerp(s_scaleY, s_targetScaleY, dt*5.0f);
    }

    imm::DrawTexture("title", gfx::GetScreenWidth()*0.5f,48.0f, s_scaleX,s_scaleY, csm::ToRad(s_angle), imm::Flip_None);
}

//
// Application
//

class RocketApp: public Application
{
public:
    void Init()
    {
        gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
        gfx::SetScreenFilter(gfx::Filter_Nearest);

        sfx::SetSoundVolume(1.0f);
        sfx::SetMusicVolume(1.0f);

        LoadAllAssets<gfx::Texture>();
        LoadAllAssets<gfx::Shader>();
        LoadAllAssets<sfx::Sound>();
        LoadAllAssets<sfx::Music>();

        CreateBackground();
        CreateRocket();
        CreateSmoke();

        LoadBitmapFont(s_font, 14,24, "font");

        s_gameState = GameState_Menu;

        sfx::PlayMusic("ambience", -1);
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(f32 dt)
    {
        // Handle locking/unlocking the mouse with debug mode.
        static bool s_lockMouse = true;
        if(IsDebugMode())
        {
            if(IsKeyPressed(KeyCode_Escape))
                s_lockMouse = !s_lockMouse;
        }
        else
        {
            s_lockMouse = true;
        }
        LockMouse(s_lockMouse);

        // If the rocket is dead then pressing R resets the game.
        if(s_gameState == GameState_Game && s_rocket.dead)
        {
            if(IsKeyPressed(KeyCode_R))
            {
                RespawnRocket();
                s_entitySpawnCooldown = 1.0f;
                s_asteroids.clear();
                s_smoke.clear();
            }
        }

        if(s_gameState == GameState_Game)
            MaybeSpawnEntity(dt);
        UpdateAsteroids(dt);
        UpdateStars(dt);
        UpdateSmoke(dt);
        UpdateRocket(dt);
        UpdateMenu(dt);
    }

    void Render(f32 dt)
    {
        RenderBackground(dt);
        RenderSmoke(dt);
        RenderAsteroids(dt);
        RenderStars(dt);
        RenderRocket(dt);
        RenderMenu(dt);
    }

    void DebugRender(f32 dt)
    {
        DebugRenderAsteroids(dt);
        DebugRenderStars(dt);
        DebugRenderRocket(dt);
    }
};

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.min = Vec2i(180,320);
    appConfig.screenSize = Vec2i(180,320);
    appConfig.app = Allocate<RocketApp>(CS_MEM_GAME);
    return appConfig;
}
