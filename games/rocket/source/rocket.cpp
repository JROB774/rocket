#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_audio.hpp"
#include "cs_utility.hpp"

#include <iomanip>
#include <sstream>

using namespace cs;

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

static bool PointInRect(Vec2 p, Rect r)
{
    return ((p.x >= r.x) && (p.y >= r.y) && (p.x < r.x+r.w) && (p.y < r.y+r.h));
}

//
// Game
//

enum GameState
{
    GameState_MainMenu,
    GameState_ScoresMenu,
    GameState_CostumesMenu,
    GameState_SettingsMenu,
    GameState_Game,
    GameState_TOTAL
};

enum Costume
{
    Costume_Red,
    Costume_Blue,
    Costume_Yellow,
    Costume_Happy,
    Costume_Sad,
    Costume_Sick,
    Costume_Meat,
    Costume_Doodle,
    Costume_Rainbow,
    Costume_Glitch,
    Costume_Random,
    Costume_TOTAL
};

enum PowerupType
{
    PowerupType_Boost,
    PowerupType_Shield,
    PowerupType_TOTAL
};

enum AsteroidType
{
    AsteroidType_Large,
    AsteroidType_Medium,
    AsteroidType_Small,
    AsteroidType_TOTAL
};

struct Rocket
{
    Vec2 pos;
    Vec2 vel;
    f32 angle;
    f32 shake;
    f32 timer;
    f32 boost;
    s32 score;
    s32 frame;
    bool shield;
    bool dead;
    Collider collider;
    Collider collector;
    sfx::SoundRef thruster;
    Costume costume;
    bool random;
    bool unlocks[Costume_TOTAL];
};

struct Powerup
{
    Vec2 pos;
    s32 frame;
    bool dead;
    Collider collider;
    PowerupType type;
};

struct Asteroid
{
    Vec2 pos;
    bool dead;
    imm::Flip flip;
    Collider collider;
    AsteroidType type;
};

static constexpr f32 k_fallSpeed = 400.0f;
static constexpr f32 k_boostMultiplier = 2.0f;
static constexpr f32 k_boostTime = 5.0f;

static GameState s_gameState;
static bool s_gamePaused;
static bool s_gameOver;
static bool s_gameResetting;
static f32 s_boostMultiplier;
static s32 s_highScore;
static u32 s_gameFrame;
static Rocket s_rocket;
static Costume s_currentCostume;
static std::vector<Powerup> s_powerups;
static std::vector<Asteroid> s_asteroids;

static void GoToMainMenu();
static void GoToScoresMenu();
static void GoToCostumesMenu();
static void GoToSettingsMenu();
static void GoToGameOverMenu();
static void GoToPauseMenu();

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

static BitmapFont s_font0;
static BitmapFont s_font1;

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
            imm::DrawTexture(font.texture, dx,dy, &bounds, color);
            ix += font.charWidth;
        }
    }
}

//
// Powerups
//

static constexpr f32 k_powerupCooldownTime = 10.0f;
static f32 s_powerupCooldown;

static void SpawnPowerup()
{
    if(s_powerupCooldown > 0.0f) return;
    Powerup powerup = {};
    powerup.frame = 0;
    powerup.dead = false;
    powerup.collider = { Vec2(0), 14.0f };
    powerup.type = CS_CAST(PowerupType, RandomS32(0,PowerupType_TOTAL-1));
    bool found = false;
    for(s32 i=0; i<10; ++i) // Try ten times to find a spot that doesn't collide with an asteroid.
    {
        powerup.pos = Vec2(RandomF32(12.0f, gfx::GetScreenWidth()-12.0f), -24.0f);
        for(auto& asteroid: s_asteroids)
            if(CheckCollision(powerup.pos, powerup.collider, asteroid.pos, asteroid.collider))
                continue;
        found = true;
        break;
    }
    if(!found) return;
    s_powerups.push_back(powerup);
    s_powerupCooldown = k_powerupCooldownTime;
}

static void UpdatePowerups(f32 dt)
{
    if(s_powerupCooldown > 0.0f)
        s_powerupCooldown -= dt;

    for(auto& powerup: s_powerups)
    {
        if(!powerup.dead)
        {
            // If we're in the rocket's collection range then move towards the rocket.
            if(CheckCollision(s_rocket.pos, s_rocket.collector, powerup.pos, powerup.collider))
                powerup.pos = csm::Lerp(powerup.pos, s_rocket.pos+s_rocket.collider.offset, Vec2(0.1f));
            else
                powerup.pos.y += (k_fallSpeed * s_boostMultiplier) * dt;
            powerup.frame++;
        }
    }

    s_powerups.erase(std::remove_if(s_powerups.begin(), s_powerups.end(),
    [](const Powerup& powerup)
    {
        return (powerup.dead || (powerup.pos.y >= (gfx::GetScreenHeight()+24.0f)));
    }),
    s_powerups.end());
}

static void RenderPowerups(f32 dt)
{
    for(auto& powerup: s_powerups)
    {
        if(!powerup.dead)
        {
            Rect clip = { 24*CS_CAST(f32,(powerup.frame % 2)),24*CS_CAST(f32, powerup.type), 24, 24 };
            imm::DrawTexture("power", powerup.pos.x, powerup.pos.y, &clip);
        }
    }
}

static void DebugRenderPowerups(f32 dt)
{
    for(auto& powerup: s_powerups)
    {
        Vec2 pos(powerup.pos + powerup.collider.offset);
        imm::DrawCircleFilled(pos.x, pos.y, powerup.collider.radius, Vec4(0,0,1,0.25f));
        imm::DrawCircleOutline(pos.x, pos.y, powerup.collider.radius, Vec4(0,0,1,1.00f));
    }
}

//
// Asteroid
//

static constexpr f32 k_asteroidMinSpinSpeed = 240.0f;
static constexpr f32 k_asteroidMaxSpinSpeed = 420.0f;

static void SpawnAsteroid()
{
    Asteroid asteroid = {};
    asteroid.pos = Vec2(RandomF32(0, gfx::GetScreenWidth()), -48.0f);
    asteroid.dead = false;
    asteroid.flip = (RandomS32() % 2 == 0) ? imm::Flip_None : imm::Flip_Horizontal;
    asteroid.type = CS_CAST(AsteroidType, RandomS32(0,AsteroidType_TOTAL-1));
    asteroid.collider.offset = Vec2(0,-2);
    switch(asteroid.type)
    {
        case(AsteroidType_Large): asteroid.collider.radius = 12.0f; break;
        case(AsteroidType_Medium): asteroid.collider.radius = 8.0f; break;
        case(AsteroidType_Small): asteroid.collider.radius = 4.0f; break;
    }
    // If we collide with a powerup then do not bother spawning.
    for(auto& powerup: s_powerups)
        if(CheckCollision(powerup.pos, powerup.collider, asteroid.pos, asteroid.collider))
            return;
    s_asteroids.push_back(asteroid);
}

static void UpdateAsteroids(f32 dt)
{
    for(auto& asteroid: s_asteroids)
    {
        asteroid.pos.y += (k_fallSpeed * s_boostMultiplier) * dt;
    }

    s_asteroids.erase(std::remove_if(s_asteroids.begin(), s_asteroids.end(),
    [](const Asteroid& asteroid)
    {
        return (asteroid.dead || (asteroid.pos.y >= (gfx::GetScreenHeight()+48.0f)));
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
// Entity Spawn
//

enum EntityType
{
    EntityType_Asteroid,
    EntityType_TOTAL
};

static constexpr f32 k_entitySpawnCooldownTime = 1.0f;
static constexpr f32 k_difficultyIncreaseInterval = 5.0f;
static constexpr s32 k_maxDifficulty = 75;

static f32 s_entitySpawnCooldown;
static f32 s_difficultyTimer;
static s32 s_difficulty;

static void MaybeSpawnEntity(f32 dt)
{
    if(s_entitySpawnCooldown > 0.0f)
    {
        s_entitySpawnCooldown -= dt;
    }
    else
    {
        // Slowly increase the difficulty as the game goes on.
        if(s_difficulty <= k_maxDifficulty)
        {
            s_difficultyTimer += dt;
            if(s_difficultyTimer >= k_difficultyIncreaseInterval)
            {
                s_difficultyTimer -= k_difficultyIncreaseInterval;
                s_difficulty++;
            }
        }

        if(RandomS32(0,1000) <= (s_difficulty * s_boostMultiplier))
            SpawnAsteroid();
        /*
        if(RandomS32(0,1000) <= 2)
            SpawnPowerup();
        */
    }
}

//
// Smoke
//

enum SmokeType
{
    SmokeType_Thruster,
    SmokeType_Blood,
    SmokeType_Small,
    SmokeType_Explosion,
    SmokeType_Stationary,
    SmokeType_SmallStationary
};

struct Smoke
{
    SmokeType type;
    Vec2 pos;
    Vec2 vel;
    s32 frame;
    f32 angle;
    f32 spin;
    f32 scale;
    f32 timer;
    f32 frameTime;
    bool spawner;
    bool dead;
};

static std::vector<Smoke> s_smoke;

static void CreateSmoke()
{
    s_smoke.reserve(4096);
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
        if(type == SmokeType_Blood) s.vel = RotateVec2(Vec2(180.0f,0), csm::ToRad(RandomF32(45.0f,135.0f)));
        s.spin = RandomF32(400,600);
        s.scale = (s.type == SmokeType_Small || s.type == SmokeType_SmallStationary) ? 0.5f : 1.0f;
        if(s.type == SmokeType_Thruster || s.type == SmokeType_Blood) s.scale *= ((s_rocket.boost > 0.0f) ? 1.5f : 1.0f);
        s.timer = 0.0f;
        s.frameTime = RandomF32(0.05f, 0.15f);
        if(type == SmokeType_Explosion) s.spawner = RandomS32(1,100) <= 10;
        else s.spawner = false;
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
        if(s.type == SmokeType_Small)
        {
            s.timer += dt;
        }
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
            case(SmokeType_Blood):
            {
                s.pos += s.vel * dt;
            } break;
            case(SmokeType_Small):
            case(SmokeType_Explosion):
            {
                s.pos += s.vel * dt;
                s.angle += s.spin * dt;
                if(s.type != SmokeType_Small)
                {
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
        Rect clip = { CS_CAST(f32, 16*s.frame), 16*CS_CAST(f32, s_rocket.costume), 16, 16 };
        imm::DrawTexture("smoke", s.pos.x, s.pos.y, s.scale,s.scale, csm::ToRad(s.angle), imm::Flip_None, &clip);
    }
}

//
// Rocket
//

static constexpr f32 k_rocketVelocityMultiplier = 25.0f;
static constexpr f32 k_rocketTerminalVelocity = 9.5f;
static constexpr f32 k_rocketMaxAngle = 25.0f;
static constexpr f32 k_rocketMaxShake = 2.0f;

static void StartThruster()
{
    if(s_rocket.thruster != sfx::k_invalidSoundRef) return;
    std::string thruster;
    if(s_rocket.boost <= 0.0f)
    {
        thruster = "thruster";
        switch(s_rocket.costume)
        {
            case(Costume_Meat): thruster = "squirt"; break;
            case(Costume_Doodle): thruster = "mouth0"; break;
            case(Costume_Rainbow): thruster = "sparkle"; break;
            case(Costume_Glitch): thruster = "static"; break;
        }
    }
    else
    {
        thruster = "boost0";
        switch(s_rocket.costume)
        {
            case(Costume_Meat): thruster = "boost1"; break;
            case(Costume_Doodle): thruster = "boost2"; break;
            case(Costume_Rainbow): thruster = "boost3"; break;
            case(Costume_Glitch): thruster = "boost4"; break;
        }
    }
    s_rocket.thruster = sfx::PlaySound(thruster, -1);
}

static void StopThruster()
{
    sfx::StopSound(s_rocket.thruster);
    s_rocket.thruster = sfx::k_invalidSoundRef;
}

static void CreateRocket()
{
    s_rocket.pos.x = (gfx::GetScreenWidth()  *  0.5f);
    s_rocket.pos.y = (gfx::GetScreenHeight() - 32.0f);
    s_rocket.vel   = Vec2(0);
    s_rocket.angle = 0.0f;
    s_rocket.shake = 0.0f;
    s_rocket.timer = 1000.0f; // Stop the explosion on start.
    s_rocket.boost = 0.0f;
    s_rocket.score = 0;
    s_rocket.frame = 0;
    s_rocket.shield = false;
    s_rocket.dead  = true;
    s_rocket.collider = { Vec2(0,-8), 8.0f };
    s_rocket.collector = { Vec2(0,-8), 40.0f };
    s_rocket.costume = Costume_Red;
    s_rocket.thruster = sfx::k_invalidSoundRef;
    s_rocket.random = (s_rocket.costume == Costume_Random);
    // These rockets are always unlocked.
    s_rocket.unlocks[Costume_Red   ] = true;
    s_rocket.unlocks[Costume_Blue  ] = true;
    s_rocket.unlocks[Costume_Yellow] = true;
    s_rocket.unlocks[Costume_Random] = true;
}

static void PowerupRocket(PowerupType type)
{
    switch(type)
    {
        case(PowerupType_Boost):
        {
            s_rocket.boost += k_boostTime;
            StopThruster();
            StartThruster();
        } break;
        case(PowerupType_Shield):
        {
            s_rocket.shield = true;
        } break;
    }
}

static void HitRocket()
{
    if(s_rocket.shield)
    {
        // @INCOMPLETE: ...
        s_rocket.shield = false;
    }
    else
    {
        SpawnSmoke(SmokeType_Explosion, s_rocket.pos.x, s_rocket.pos.y, RandomS32(20,40));
        StopThruster();
        std::string explosion = "explosion";
        switch(s_rocket.costume)
        {
            case(Costume_Meat): explosion = "splat"; break;
            case(Costume_Doodle): explosion = "mouth2"; break;
            case(Costume_Rainbow): explosion = "ignite"; break;
            case(Costume_Glitch): explosion = "glitch"; break;
        }
        sfx::PlaySound(explosion);
        s_rocket.timer = 0.0f;
        s_rocket.boost = 0.0f;
        s_rocket.dead = true;
    }
}

static void UpdateRocket(f32 dt)
{
    if(s_gameState != GameState_Game) return;

    // Change costume.
    if(CS_DEBUG)
    {
        Costume oldCostume = s_rocket.costume;
        if(IsKeyPressed(KeyCode_1)) s_rocket.costume = Costume_Red;
        if(IsKeyPressed(KeyCode_2)) s_rocket.costume = Costume_Blue;
        if(IsKeyPressed(KeyCode_3)) s_rocket.costume = Costume_Yellow;
        if(IsKeyPressed(KeyCode_4)) s_rocket.costume = Costume_Happy;
        if(IsKeyPressed(KeyCode_5)) s_rocket.costume = Costume_Sad;
        if(IsKeyPressed(KeyCode_6)) s_rocket.costume = Costume_Sick;
        if(IsKeyPressed(KeyCode_7)) s_rocket.costume = Costume_Meat;
        if(IsKeyPressed(KeyCode_8)) s_rocket.costume = Costume_Doodle;
        if(IsKeyPressed(KeyCode_9)) s_rocket.costume = Costume_Rainbow;
        if(IsKeyPressed(KeyCode_0)) s_rocket.costume = Costume_Glitch;
        // Need to reset the thruster sound.
        if(oldCostume != s_rocket.costume)
        {
            if(!s_rocket.dead)
            {
                StopThruster();
                StartThruster();
            }
        }
        // Instant kill.
        if(IsKeyPressed(KeyCode_R))
            HitRocket();
    }

    s_rocket.timer += dt;

    if(s_rocket.costume == Costume_Doodle)
    {
        if(s_gameFrame % 3 == 0)
            s_rocket.frame++;
    }
    else
    {
        s_rocket.frame++;
    }
    // Make sure the frame stays in-bounds of the animation.
    if(s_rocket.boost > 0.0f)
    {
        if(s_rocket.frame >= 3)
        {
            s_rocket.frame = 0;
        }
    }
    else
    {
        if(s_rocket.frame >= 5)
        {
            s_rocket.frame = 0;
        }
    }

    // Handle smoothly lerping the boost multiplier.
    f32 boostTarget = (s_rocket.boost > 0.0f) ? k_boostMultiplier : 1.0f;
    s_boostMultiplier = csm::Lerp(s_boostMultiplier, boostTarget, 0.05f);

    if(!s_rocket.dead)
    {
        if(s_rocket.boost > 0.0f)
        {
            s_rocket.boost -= dt;
            if(s_rocket.boost <= 0.0f)
            {
                StopThruster();
                StartThruster();
            }
        }

        if(IsMouseLocked() && (s_gameState == GameState_Game))
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
                    std::string whoosh = "whoosh";
                    switch(s_rocket.costume)
                    {
                        case(Costume_Meat): whoosh = "squelch"; SpawnSmoke(SmokeType_Small, s_rocket.pos.x, s_rocket.pos.y, RandomS32(2,5)); break;
                        case(Costume_Doodle): whoosh = "mouth1"; break;
                        case(Costume_Rainbow): whoosh = "magic"; break;
                        case(Costume_Glitch): whoosh = "fuzz"; break;
                    }
                    sfx::PlaySound(whoosh);
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

        if(s_rocket.timer >= 0.05f)
        {
            SmokeType smokeType = (s_rocket.costume == Costume_Meat) ? SmokeType_Blood : SmokeType_Thruster;
            s32 smokeCount = 1;
            if(s_rocket.costume == Costume_Meat) smokeCount = 2;
            if(s_rocket.costume == Costume_Rainbow) smokeCount = 2;
            SpawnSmoke(smokeType, s_rocket.pos.x+RandomF32(-3.0f,3.0f), s_rocket.pos.y+20.0f, CS_CAST(s32,(smokeCount * s_boostMultiplier)));
            s_rocket.timer -= 0.05f;
        }

        // Handle collision checks.
        if(s_gameState == GameState_Game)
        {
            // Asteroids.
            for(auto& asteroid: s_asteroids)
            {
                if(CheckCollision(s_rocket.pos, s_rocket.collider, asteroid.pos, asteroid.collider))
                {
                    if(s_rocket.boost <= 0.0f)
                    {
                        HitRocket();
                    }
                    else
                    {
                        // If we're boosting then destroy the asteroid instead.
                        asteroid.dead = true;
                        // @INCOMPLETE: ...
                    }
                }
            }
            // Powerups.
            for(auto& powerup: s_powerups)
            {
                if(!powerup.dead)
                {
                    if(CheckCollision(s_rocket.pos, s_rocket.collider, powerup.pos, powerup.collider))
                    {
                        PowerupRocket(powerup.type);
                        powerup.dead = true;
                    }
                }
            }
        }

        // Increment the score.
        if((s_gameState == GameState_Game) && !s_gameResetting)
        {
            s32 oldScore = s_rocket.score;
            s_rocket.score += CS_CAST(s32, (2.0f * s_boostMultiplier));
            if(s_highScore != 0 && oldScore <= s_highScore && s_rocket.score > s_highScore)
                sfx::PlaySound("highscore");
        }
    }
}

static void RenderRocket(f32 dt)
{
    if(s_gameState != GameState_Game) return;

    if(s_rocket.dead)
    {
        // Draw the explosion.
        f32 frame = floorf(s_rocket.timer / 0.04f);
        if(frame < 13)
        {
            Rect clip = { 96*frame, 96*CS_CAST(f32, s_rocket.costume), 96, 96 };
            imm::DrawTexture("explosion", s_rocket.pos.x, s_rocket.pos.y, &clip);
            if(s_rocket.costume != Costume_Doodle)
            {
                imm::DrawTexture("explosion", s_rocket.pos.x-20, s_rocket.pos.y-10, 0.5f,0.5f, 0.0f, imm::Flip_None, &clip);
                imm::DrawTexture("explosion", s_rocket.pos.x+10, s_rocket.pos.y+30, 0.5f,0.5f, 0.0f, imm::Flip_None, &clip);
            }
        }
    }
    else
    {
        // Draw the rocket.
        Rect clip = { 48*CS_CAST(f32,s_rocket.frame), 96*CS_CAST(f32,s_rocket.costume), 48, 96 };
        if(s_rocket.boost > 0.0f) clip.x += 48*5;
        f32 angle = csm::ToRad(s_rocket.angle + s_rocket.shake);
        imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, imm::Flip_None, &clip);
        // Draw the shield.
        if(s_rocket.shield)
        {
            Vec2 pos = s_rocket.pos + s_rocket.collider.offset;
            imm::DrawCircleFilled(pos.x,pos.y, 32.0f, Vec4(1,1,1,0.25f));
            imm::DrawCircleOutline(pos.x,pos.y, 32.0f, Vec4(1,1,1,0.50f));
            imm::DrawCircleFilled(pos.x-16.0f,pos.y-16.0f, 5.0f, Vec4(1,1,1,1));
        }
    }

    // Draw the score.
    bool beatHighScore = ((s_rocket.score > s_highScore) && (s_highScore != 0));
    BitmapFont* font = (beatHighScore) ? &s_font1 : &s_font0;
    std::string scoreStr = std::to_string(s_rocket.score);
    f32 textWidth = GetTextLineWidth(*font, scoreStr);
    if(beatHighScore) scoreStr += "!";
    f32 screenWidth = gfx::GetScreenWidth();
    f32 screenHeight = gfx::GetScreenHeight();
    DrawBitmapFont(*font, (screenWidth-textWidth)*0.5f,4.0f, scoreStr);
}

static void DebugRenderRocket(f32 dt)
{
    Vec2 posA(s_rocket.pos + s_rocket.collector.offset);
    Vec2 posB(s_rocket.pos + s_rocket.collider.offset);

    imm::DrawCircleFilled(posA.x, posA.y, s_rocket.collector.radius, Vec4(0,0,1,0.25f));
    imm::DrawCircleOutline(posA.x, posA.y, s_rocket.collector.radius, Vec4(0,0,1,1.00f));
    imm::DrawCircleFilled(posB.x, posB.y, s_rocket.collider.radius, Vec4(0,1,0,0.25f));
    imm::DrawCircleOutline(posB.x, posB.y, s_rocket.collider.radius, Vec4(0,1,0,1.00f));
}

//
// Transition
//

static GameState s_resetTarget = GameState_Game;
static f32 s_fadeHeight = 0.0f;
static bool s_fadeOut = false;

static void ResetGame(GameState target)
{
    s_resetTarget = target;
    s_gameResetting = true;
    s_fadeOut = true;
    s_fadeHeight = 0.0f;
}

static void RenderTransition(f32 dt)
{
    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();

    f32 speed = 1400.0f;
    Vec4 color = Vec4(0,0,0,1);

    if(s_fadeOut)
    {
        s_fadeHeight += speed * dt;
        f32 y = screenH - s_fadeHeight;
        imm::DrawRectFilled(0,y,screenW,y+s_fadeHeight, color);
        imm::DrawTexture("transition", screenW*0.5f, y-8.0f);

        if(s_fadeHeight >= gfx::GetScreenHeight())
        {
            s_gameState = s_resetTarget;
            if(s_rocket.score > s_highScore)
                s_highScore = s_rocket.score;
            s_rocket.pos = Vec2(screenW*0.5f, screenH-32.0f);
            s_rocket.vel = Vec2(0);
            s_rocket.score = 0;
            s_rocket.timer = 0.0f;
            s_rocket.boost = 0.0f;
            s_rocket.shield = false;
            s_rocket.dead = false;
            s_powerupCooldown = k_powerupCooldownTime;
            s_entitySpawnCooldown = k_entitySpawnCooldownTime;
            s_boostMultiplier = 1.0f;
            s_difficultyTimer = 0.0f;
            s_difficulty = 50;
            s_powerups.clear();
            s_asteroids.clear();
            s_smoke.clear();
            s_gamePaused = false;
            s_fadeOut = false;
            if(s_gameState == GameState_Game)
                StartThruster();
            if(s_gameState == GameState_MainMenu)
                GoToMainMenu();
            // Pick a random costume.
            if(s_rocket.random)
            {
                Costume costume = Costume_Random;
                while((!s_rocket.unlocks[costume]) || (costume == Costume_Random) || (costume == s_rocket.costume))
                    costume = CS_CAST(Costume, RandomS32(Costume_Red,Costume_Glitch));
                s_rocket.costume = costume;
            }
        }
    }
    else
    {
        s_fadeHeight -= speed * dt;
        imm::DrawRectFilled(0,0,screenW,s_fadeHeight, color);
        imm::DrawTexture("transition", screenW*0.5f, s_fadeHeight+8.0f, 1.0f,1.0f, 0.0f, imm::Flip_Vertical);

        if(s_fadeHeight <= 0.0f)
        {
            s_gameResetting = false;
        }
    }
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

static void UpdateBackground(f32 dt)
{
    f32 screenHeight = gfx::GetScreenHeight();
    for(s32 i=0; i<k_backCount; ++i)
    {
        s_backOffset[i] += (s_backSpeed[i] * s_boostMultiplier) * dt;
        if(s_backOffset[i] >= screenHeight * 1.5f)
            s_backOffset[i] -= screenHeight;
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
        imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i], &clip, color);
        imm::DrawTexture("back", screenWidth*0.5f,s_backOffset[i]-screenHeight, &clip, color);
        clip.x += 180.0f;
    }
}

//
// Cursor
//

static void RenderCursor(f32 dt)
{
    if(s_gameState != GameState_Game || s_gamePaused)
    {
        Vec2 pos = GetScreenMousePos();
        f32 x = roundf(pos.x);
        f32 y = roundf(pos.y);
        imm::DrawTexture("cursor", x,y);
    }
}

//
// Menu System
//

struct MenuOption;

typedef void(*MenuOptionAction)(MenuOption& option);

enum MenuOptionType
{
    MenuOptionType_Button,
    MenuOptionType_Toggle,
    MenuOptionType_Slider
};

struct MenuOption
{
    MenuOptionAction action = NULL;
    MenuOptionType type = MenuOptionType_Button;
    Rect bounds = {};
    Rect clip = {};
    bool selected = false;
    f32 scale = 1.0f;
    f32 targetScale = 1.0f;
    bool toggle = false;
    f32 slider = 1.0f;
};

static void UpdateMenuOptions(MenuOption* options, size_t count, f32 dt)
{
    Vec2 mouse = GetScreenMousePos();
    for(size_t i=0; i<count; ++i)
    {
        MenuOption& option = options[i];
        bool oldSelected = option.selected;
        option.selected = PointInRect(mouse, option.bounds);
        option.targetScale = (option.selected) ? 1.33f : 1.0f;
        option.scale = csm::Lerp(option.scale, option.targetScale, 0.5f);

        // If the option went from non-selected to selected then play a sound.
        if(option.selected && (oldSelected != option.selected))
            sfx::PlaySound("click");
    }

    // Handle the interaction logic based on what type of option it is.
    bool pressed = IsMouseButtonPressed(MouseButton_Left);
    if(pressed)
    {
        for(size_t i=0; i<count; ++i)
        {
            MenuOption& option = options[i];
            if(option.selected)
            {
                sfx::PlaySound("select");
                option.scale = 2.0f;
                switch(option.type)
                {
                    case(MenuOptionType_Toggle):
                    {
                        option.toggle = !option.toggle;
                    } break;
                    case(MenuOptionType_Slider):
                    {
                        // @INCOMPLETE: ...
                    } break;
                }
                // Call the user-defined action on interaction.
                if(option.action)
                {
                    option.action(option);
                }
            }
        }
    }
}

static void RenderMenuOptions(MenuOption* options, size_t count, f32 dt)
{
    static f32 s_angle = 0.0f;
    static f32 s_timer = 0.0f;

    f32 halfW = gfx::GetScreenWidth() * 0.5f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    for(size_t i=0; i<count; ++i)
    {
        MenuOption& option = options[i];

        f32 xPos  = option.bounds.x + (option.bounds.w * 0.5f);
        f32 yPos  = option.bounds.y + (option.bounds.h * 0.5f);
        f32 scale = option.scale;
        f32 angle = 0.0f;
        Rect clip = option.clip;

        if(option.selected)
        {
            clip.x += 128.0f;
            angle = -s_angle;
        }
        if(option.type == MenuOptionType_Toggle)
        {
            if(!option.toggle)
            {
                clip.y += 24.0f;
            }
        }

        imm::DrawTexture("menu", xPos,yPos, scale,scale, csm::ToRad(angle), imm::Flip_None, &clip);
    }
}

static void ResetMenuOptions(MenuOption* options, size_t count)
{
    for(size_t i=0; i<count; ++i)
    {
        options[i].selected = false;
        options[i].targetScale = 1.0f;
        options[i].scale = 1.0f;
    }
}

//
// Main Menu
//

static void MainMenuActionStart(MenuOption& option)
{
    s_entitySpawnCooldown = k_entitySpawnCooldownTime;
    ResetGame(GameState_Game);
}

static void MainMenuActionScores(MenuOption& option)
{
    GoToScoresMenu();
}

static void MainMenuActionCostumes(MenuOption& option)
{
    GoToCostumesMenu();
}

static void MainMenuActionSettings(MenuOption& option)
{
    GoToSettingsMenu();
}

static void MainMenuActionExit(MenuOption& option)
{
    GetAppConfig().app->m_running = false;
}

enum MainMenuOptionID
{
    MainMenuOption_Start,
    MainMenuOption_Scores,
    MainMenuOption_Costumes,
    MainMenuOption_Settings,
    MainMenuOption_Exit,
    MainMenuOption_TOTAL
};

static MenuOption s_mainMenuOptions[MainMenuOption_TOTAL]
{
{ MainMenuActionStart,    MenuOptionType_Button, { 0.0f,128.0f,      180.0f,24.0f }, { 0, 192,128,24 } },
{ MainMenuActionScores,   MenuOptionType_Button, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0, 216,128,24 } },
{ MainMenuActionCostumes, MenuOptionType_Button, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0, 240,128,24 } },
{ MainMenuActionSettings, MenuOptionType_Button, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0, 264,128,24 } },
{ MainMenuActionExit,     MenuOptionType_Button, { 0.0f,128.0f+96.0f,180.0f,24.0f }, { 0, 288,128,24 } }
};

static void UpdateMainMenu(f32 dt)
{
    if(s_gameState != GameState_MainMenu) return;
    UpdateMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        MainMenuActionExit(s_mainMenuOptions[MainMenuOption_Exit]);
}

static void RenderMainMenu(f32 dt)
{
    if(s_gameState != GameState_MainMenu) return;
    RenderMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL, dt);

    Rect titleClip  = { 0,   0,256,64 };
    Rect authorClip = { 0,1056,256,24 };

    static f32 s_scaleX = 1.0f;
    static f32 s_scaleY = 1.0f;
    static f32 s_angle  = 0.0f;
    static f32 s_timer  = 0.0f;

    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    s_scaleX = SinRange(0.8f, 1.0f, s_timer*1.5f);
    s_scaleY = SinRange(0.8f, 1.0f, s_timer*2.0f);

    imm::DrawTexture("menu", halfW,48.0f, s_scaleX,s_scaleY, csm::ToRad(s_angle), imm::Flip_None, &titleClip);
    imm::DrawTexture("menu", halfW,screenH-12.0f, &authorClip);
}

static void GoToMainMenu()
{
    s_gameState = GameState_MainMenu;
    ResetMenuOptions(s_mainMenuOptions, MainMenuOption_TOTAL);
}

//
// Scores Menu
//

static void ScoresMenuActionBack(MenuOption& option)
{
    GoToMainMenu();
}

enum ScoresMenuOption
{
    ScoresMenuOption_Back,
    ScoresMenuOption_TOTAL
};

static MenuOption s_scoresMenuOptions[ScoresMenuOption_TOTAL]
{
{ ScoresMenuActionBack, MenuOptionType_Button, { 0.0f,288.0f,180.0f,24.0f }, { 0,576,128,24 } }
};

static void UpdateScoresMenu(f32 dt)
{
    if(s_gameState != GameState_ScoresMenu) return;
    UpdateMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        ScoresMenuActionBack(s_scoresMenuOptions[ScoresMenuOption_Back]);
}

static void RenderScoresMenu(f32 dt)
{
    if(s_gameState != GameState_ScoresMenu) return;
    RenderMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL, dt);
    Rect titleClip = { 0,64,256,32 };
    imm::DrawTexture("menu", gfx::GetScreenWidth()*0.5f,24.0f, &titleClip);
}

static void GoToScoresMenu()
{
    s_gameState = GameState_ScoresMenu;
    ResetMenuOptions(s_scoresMenuOptions, ScoresMenuOption_TOTAL);
}

//
// Costumes Menu
//

static constexpr f32 k_costumeLockedTextOffset = 192.0f;
static f32 s_costumeScale = 1.0f;

static void CostumesMenuActionLeft(MenuOption& option)
{
    s32 costume = CS_CAST(s32, s_rocket.costume);
    if(costume > 0) costume--;
    else costume = Costume_TOTAL-1;
    s_rocket.costume = CS_CAST(Costume, costume);
    s_costumeScale = 1.5f;
}

static void CostumesMenuActionRight(MenuOption& option)
{
    s32 costume = CS_CAST(s32, s_rocket.costume);
    if(costume < Costume_TOTAL-1) costume++;
    else costume = 0;
    s_rocket.costume = CS_CAST(Costume, costume);
    s_costumeScale = 1.5f;
}

static void CostumesMenuActionBack(MenuOption& option)
{
    if(!s_rocket.unlocks[s_rocket.costume]) // If the selected costume is locked reset to the last costume.
        s_rocket.costume = s_currentCostume;
    s_rocket.random = (s_rocket.costume == Costume_Random);
    GoToMainMenu();
}

enum CostumesMenuOption
{
    CostumesMenuOption_Left,
    CostumesMenuOption_Right,
    CostumesMenuOption_Back,
    CostumesMenuOption_TOTAL
};

static MenuOption s_costumesMenuOptions[CostumesMenuOption_TOTAL]
{
{ CostumesMenuActionLeft,  MenuOptionType_Button, {   8.0f,136.0f, 48.0f,48.0f }, { 0,1080,128,24 } },
{ CostumesMenuActionRight, MenuOptionType_Button, { 124.0f,136.0f, 48.0f,48.0f }, { 0,1104,128,24 } },
{ CostumesMenuActionBack,  MenuOptionType_Button, {   0.0f,288.0f,180.0f,24.0f }, { 0, 576,128,24 } }
};

static void UpdateCostumesMenu(f32 dt)
{
    if(s_gameState != GameState_CostumesMenu) return;
    UpdateMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        CostumesMenuActionBack(s_costumesMenuOptions[CostumesMenuOption_Back]);
}

static void RenderCostumesMenu(f32 dt)
{
    if(s_gameState != GameState_CostumesMenu) return;
    RenderMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL, dt);

    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    Vec2 mouse = GetScreenMousePos();

    // Draw the title.
    Rect titleClip = { 0,96,256,32 };
    imm::DrawTexture("menu", halfW,24.0f, &titleClip);

    // Draw the costume.
    f32 costumeOffset = 64 * CS_CAST(f32, s_rocket.costume);
    f32 nameOffset = 24 * CS_CAST(f32, s_rocket.costume);

    Rect costumeClip = { 64+costumeOffset,0,64,64 };
    Rect nameClip = { 0,624+nameOffset,256,24 };

    if(!s_rocket.unlocks[s_rocket.costume])
    {
        costumeClip.x = 0.0f;
        nameClip.y += k_costumeLockedTextOffset;
    }

    s_costumeScale = csm::Lerp(s_costumeScale, 1.0f, 0.5f);

    imm::DrawTexture("costume", halfW,halfH, s_costumeScale,s_costumeScale, 0.0f, imm::Flip_None, &costumeClip);
    imm::DrawTexture("menu", halfW,halfH+48, &nameClip);
}

static void GoToCostumesMenu()
{
    s_gameState = GameState_CostumesMenu;
    ResetMenuOptions(s_costumesMenuOptions, CostumesMenuOption_TOTAL);
    if(s_rocket.random)
        s_rocket.costume = Costume_Random;
    s_currentCostume = s_rocket.costume;
    s_costumeScale = 1.0f;
}

//
// Settings Menu
//

static void SettingsMenuActionSound(MenuOption& option)
{
    // @INCOMPLETE: ...
}

static void SettingsMenuActionMusic(MenuOption& option)
{
    // @INCOMPLETE: ...
}

static void SettingsMenuActionFullscreen(MenuOption& option)
{
    FullscreenWindow(option.toggle);
}

static void SettingsMenuActionVSync(MenuOption& option)
{
    // @INCOMPLETE: ...
}

static void SettingsMenuActionResetSave(MenuOption& option)
{
    // @INCOMPLETE: ...
}

static void SettingsMenuActionBack(MenuOption& option)
{
    GoToMainMenu();
}

enum SettingsMenuOption
{
    SettingsMenuOption_Sound,
    SettingsMenuOption_Music,
    SettingsMenuOption_Fullscreen,
    SettingsMenuOption_VSync,
    SettingsMenuOption_ResetSave,
    SettingsMenuOption_Back,
    SettingsMenuOption_TOTAL
};

static MenuOption s_settingsMenuOptions[SettingsMenuOption_TOTAL]
{
{ SettingsMenuActionSound,      MenuOptionType_Slider, { 0.0f,128.0f,      180.0f,24.0f }, { 0,360,128,24 } },
{ SettingsMenuActionMusic,      MenuOptionType_Slider, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0,384,128,24 } },
{ SettingsMenuActionFullscreen, MenuOptionType_Toggle, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0,408,128,24 } },
{ SettingsMenuActionVSync,      MenuOptionType_Toggle, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0,456,128,24 } },
{ SettingsMenuActionResetSave,  MenuOptionType_Button, { 0.0f,128.0f+96.0f,180.0f,24.0f }, { 0,504,128,24 } },
{ SettingsMenuActionBack,       MenuOptionType_Button, { 0.0f,288.0f,      180.0f,24.0f }, { 0,576,128,24 } }
};

static void UpdateSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    s_settingsMenuOptions[SettingsMenuOption_Fullscreen].toggle = IsFullscreen();
    UpdateMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_Escape))
        SettingsMenuActionBack(s_settingsMenuOptions[SettingsMenuOption_Back]);
}

static void RenderSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    RenderMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL, dt);
    Rect titleClip = { 0,128,256,32 };
    imm::DrawTexture("menu", gfx::GetScreenWidth()*0.5f,24.0f, &titleClip);
}

static void GoToSettingsMenu()
{
    s_gameState = GameState_SettingsMenu;
    ResetMenuOptions(s_settingsMenuOptions, SettingsMenuOption_TOTAL);
}

//
// Game Over Menu
//

static void UpdateGameOverMenu(f32 dt)
{
    // @INCOMPLETE: ...
}

static void RenderGameOverMenu(f32 dt)
{
    // @INCOMPLETE: ...
}

static void GoToGameOverMenu()
{
    // @INCOMPLETE: ...
}

//
// Pause Menu
//

static void PauseGame()
{
    s_gamePaused = true;
    sfx::PlaySound("pause");
    sfx::PauseMusic();
    StopThruster();
    GoToPauseMenu();
}

static void PauseMenuActionResume(MenuOption& option)
{
    s_gamePaused = false;
    sfx::PlaySound("pause");
    sfx::ResumeMusic();
    StartThruster();
}

static void PauseMenuActionMenu(MenuOption& option)
{
    ResetGame(GameState_MainMenu);
}

enum PauseMenuOption
{
    PauseMenuOption_Resume,
    PauseMenuOption_Menu,
    PauseMenuOption_TOTAL
};

static MenuOption s_pauseMenuOptions[PauseMenuOption_TOTAL]
{
{ PauseMenuActionResume, MenuOptionType_Button, { 0.0f,200.0f,180.0f,24.0f }, { 0,312,128,24 } },
{ PauseMenuActionMenu,   MenuOptionType_Button, { 0.0f,224.0f,180.0f,24.0f }, { 0,336,128,24 } }
};

static void UpdatePauseMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || s_rocket.dead) return;

    // Toggle pause menu.
    if(IsKeyPressed(KeyCode_Escape))
    {
        s_gamePaused = !s_gamePaused;
        if(!s_gamePaused) PauseMenuActionResume(s_pauseMenuOptions[PauseMenuOption_Resume]);
        else PauseGame();
    }

    // Do pause menu.
    if(s_gamePaused)
    {
        UpdateMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL, dt);
    }
}

static void RenderPauseMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(!s_gamePaused) return;

    static Rect s_pauseClip = { 0,160,256,32 };

    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    imm::DrawRectFilled(0,0,screenW,screenH, Vec4(0,0,0,0.5f));
    imm::DrawTexture("menu", halfW,halfH-40, &s_pauseClip);
    RenderMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL, dt);
}

static void GoToPauseMenu()
{
    ResetMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL);
}

//
// Application
//

static bool s_lockMouse = true;
static bool s_showMouse = false;

static void GameStateDebugUiCallback(bool& open)
{
    ImGui::Text("Lock Mouse: %s", (s_lockMouse) ? "True" : "False");
    ImGui::Text("Show Mouse: %s", (s_showMouse) ? "True" : "False");
    ImGui::Separator();
    ImGui::Text("Particle Count: %d", s_smoke.size());
    ImGui::Text("Powerup Count: %d", s_powerups.size());
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
    ImGui::Text("Boost: %f", s_rocket.boost);
    ImGui::Text("Shield: %s", (s_rocket.shield) ? "True" : "False");
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

        LoadBitmapFont(s_font0, 14,24, "font0");
        LoadBitmapFont(s_font1, 14,24, "font1");

        sfx::PlayMusic("music", -1);

        s_boostMultiplier = 1.0f;
        s_gameState = GameState_MainMenu;
        s_gamePaused = false;
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(f32 dt)
    {
        // Handle locking/unlocking and showing/hiding the mouse with debug mode.
        s_lockMouse = (!s_gamePaused && (s_gameState == GameState_Game));
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
            // If the rocket is dead then pressing R resets the game.
            if(s_gameState == GameState_Game && s_rocket.dead && !s_gameResetting)
                if(IsMouseButtonPressed(MouseButton_Left))
                    ResetGame(GameState_Game);

            if((s_gameState == GameState_Game) && !s_gameResetting)
                MaybeSpawnEntity(dt);
            UpdateBackground(dt);
            UpdatePowerups(dt);
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
        RenderPowerups(dt);
        RenderMainMenu(dt);
        RenderScoresMenu(dt);
        RenderCostumesMenu(dt);
        RenderSettingsMenu(dt);
        RenderGameOverMenu(dt);
        RenderPauseMenu(dt);
        RenderCursor(dt);
        RenderTransition(dt);
    }

    void DebugRender(f32 dt)
    {
        DebugRenderPowerups(dt);
        DebugRenderAsteroids(dt);
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
