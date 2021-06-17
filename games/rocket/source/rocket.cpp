#include "cs_platform.hpp"
#include "cs_application.hpp"
#include "cs_graphics.hpp"
#include "cs_audio.hpp"
#include "cs_utility.hpp"

#include <iomanip>
#include <stack>
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
    u32 score;
    s32 frame;
    bool dead;
    Collider collider;
    Collider collector;
    sfx::SoundRef thruster;
    bool random;
    // Save data.
    Costume costume;
    bool unlocks[Costume_TOTAL];
    u32 highscores[10];
};

struct Asteroid
{
    Vec2 pos;
    bool dead;
    imm::Flip flip;
    Collider collider;
    AsteroidType type;
};

struct Unlock
{
    Costume costume;
    u32 score;
};

static constexpr f32 k_fallSpeed = 400.0f;
static constexpr f32 k_boostMultiplier = 2.0f;
static constexpr f32 k_boostTime = 5.0f;

static constexpr Unlock k_unlocks[]
{
{ Costume_Happy,    1000 },
{ Costume_Sad,      2000 },
{ Costume_Sick,     3000 },
{ Costume_Meat,     5000 },
{ Costume_Doodle,  10000 },
{ Costume_Rainbow, 15000 },
{ Costume_Glitch,  25000 }
};

static GameState s_gameState;
static bool s_gamePaused;
static bool s_gameResetting;
static f32 s_boostMultiplier;
static u32 s_gameFrame;
static Rocket s_rocket;
static Costume s_currentCostume;
static std::vector<Asteroid> s_asteroids;
static std::stack<Costume> s_gameOverUnlocks;

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
static BitmapFont s_bigFont0;
static BitmapFont s_bigFont1;

static void LoadBitmapFont(BitmapFont& font, f32 cw, f32 ch, std::string texture)
{
    font.texture = texture;
    font.charWidth = cw;
    font.charHeight = ch;

    for(s32 iy=0; iy<3; ++iy)
        for(s32 ix=0; ix<32; ++ix)
            font.bounds[iy*32+ix] = { CS_CAST(f32,ix)*cw, CS_CAST(f32,iy)*ch, cw, ch };
}

static f32 GetCharWidth(BitmapFont& font, char c)
{
    if(font.charWidth == 14) // Small font.
    {
        switch(c)
        {
            case('0'): return 13;
            case('1'): return 10;
            case('2'): return 13;
            case('3'): return 13;
            case('4'): return 13;
            case('5'): return 13;
            case('6'): return 13;
            case('7'): return 13;
            case('8'): return 13;
            case('9'): return 13;
        }
    }
    if(font.charWidth == 24) // Big font.
    {
        switch(c)
        {
            case('0'): return 21;
            case('1'): return 14;
            case('2'): return 21;
            case('3'): return 21;
            case('4'): return 23;
            case('5'): return 20;
            case('6'): return 21;
            case('7'): return 21;
            case('8'): return 21;
            case('9'): return 20;
        }
    }
    return font.charWidth;
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
            lineWidth += GetCharWidth(font, text[i]);
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

    Vec2 anchor(0.0f);

    for(size_t i=0; i<text.length(); ++i)
    {
        if(text[i] == '\n')
        {
            ix = roundf(x + (GetTextLineWidth(font, text, 0)*0.5f) - (GetTextLineWidth(font, text, line+1)*0.5f));
            iy += font.charHeight;
            line++;
        }
        else
        {
            Rect bounds = font.bounds[CS_CAST(u8,text.at(i))];
            imm::DrawTexture(font.texture, ix,iy, 1.0f,1.0f, 0.0f, imm::Flip_None, &anchor, &bounds, color);
            ix += roundf(GetCharWidth(font, text[i]));
        }
    }
}

//
// Save Game
//

// The save file format is as follows:
//
// u16     - Version Number
// u8      - Current Costume
// u8      - Costume Unlocks as Bitflags
// u32[10] - Highscores

static constexpr const char* k_saveFileName = "save.dat";
static constexpr u16 k_saveVersion = 0;

enum UnlocksFlags : u8
{
    UnlockFlags_None    = 0,
    UnlockFlags_Happy   = 1 << 0,
    UnlockFlags_Sad     = 1 << 1,
    UnlockFlags_Sick    = 1 << 2,
    UnlockFlags_Meat    = 1 << 3,
    UnlockFlags_Doodle  = 1 << 4,
    UnlockFlags_Rainbow = 1 << 5,
    UnlockFlags_Glitch  = 1 << 6
};

static void SaveGame()
{
    std::string fileName = GetDataPath() + k_saveFileName;
    FILE* file = fopen(fileName.c_str(), "wb");
    if(!file)
        CS_DEBUG_LOG("Failed to save game data!");
    else
    {
        CS_DEFER { fclose(file); };

        u8 currentCostume = (s_rocket.random) ? CS_CAST(u8, Costume_Random) : CS_CAST(u8, s_rocket.costume);
        u8 unlockFlags = UnlockFlags_None;

        if(s_rocket.unlocks[Costume_Happy  ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Happy);
        if(s_rocket.unlocks[Costume_Sad    ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Sad);
        if(s_rocket.unlocks[Costume_Sick   ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Sick);
        if(s_rocket.unlocks[Costume_Meat   ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Meat);
        if(s_rocket.unlocks[Costume_Doodle ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Doodle);
        if(s_rocket.unlocks[Costume_Rainbow]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Rainbow);
        if(s_rocket.unlocks[Costume_Glitch ]) CS_SET_FLAGS(unlockFlags, UnlockFlags_Glitch);

        fwrite(&k_saveVersion, sizeof(k_saveVersion), 1, file);
        fwrite(&currentCostume, sizeof(currentCostume), 1, file);
        fwrite(&unlockFlags, sizeof(unlockFlags), 1, file);
        for(s32 i=0; i<10; ++i)
            fwrite(&s_rocket.highscores[i], sizeof(s_rocket.highscores[i]), 1, file);
    }
}

static void LoadGame()
{
    // These rockets are always unlocked.
    s_rocket.unlocks[Costume_Red   ] = true;
    s_rocket.unlocks[Costume_Blue  ] = true;
    s_rocket.unlocks[Costume_Yellow] = true;
    s_rocket.unlocks[Costume_Random] = true;

    // Load data if available.
    std::string fileName = GetDataPath() + k_saveFileName;
    if(DoesFileExist(fileName))
    {
        FILE* file = fopen(fileName.c_str(), "rb");
        if(!file)
            CS_DEBUG_LOG("Failed to load game data!");
        else
        {
            CS_DEFER { fclose(file); };

            u16 version;
            u8 currentCostume;
            u8 unlockFlags;

            fread(&version, sizeof(version), 1, file);
            if(version == 0)
            {
                fread(&currentCostume, sizeof(currentCostume), 1, file);
                fread(&unlockFlags, sizeof(unlockFlags), 1, file);
                for(s32 i=0; i<10; ++i)
                    fread(&s_rocket.highscores[i], sizeof(s_rocket.highscores[i]), 1, file);

                s_rocket.costume = CS_CAST(Costume, currentCostume);
                s_rocket.random = (s_rocket.costume == Costume_Random);

                s_rocket.unlocks[Costume_Happy  ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Happy);
                s_rocket.unlocks[Costume_Sad    ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Sad);
                s_rocket.unlocks[Costume_Sick   ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Sick);
                s_rocket.unlocks[Costume_Meat   ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Meat);
                s_rocket.unlocks[Costume_Doodle ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Doodle);
                s_rocket.unlocks[Costume_Rainbow] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Rainbow);
                s_rocket.unlocks[Costume_Glitch ] = CS_CHECK_FLAGS(unlockFlags, UnlockFlags_Glitch);
            }
        }
    }
}

static void ResetSave()
{
    s_rocket.unlocks[Costume_Happy  ] = false;
    s_rocket.unlocks[Costume_Sad    ] = false;
    s_rocket.unlocks[Costume_Sick   ] = false;
    s_rocket.unlocks[Costume_Meat   ] = false;
    s_rocket.unlocks[Costume_Doodle ] = false;
    s_rocket.unlocks[Costume_Rainbow] = false;
    s_rocket.unlocks[Costume_Glitch ] = false;
    for(s32 i=0; i<10; ++i)
        s_rocket.highscores[i] = 0;
    s_rocket.costume = Costume_Red;
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
        imm::DrawTexture("asteroid", asteroid.pos.x, asteroid.pos.y, 1.0f, 1.0f, 0.0f, asteroid.flip, NULL, &clip);
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

static constexpr f32 k_entitySpawnCooldownTime = 2.0f;
static constexpr f32 k_difficultyIncreaseInterval = 5.0f;
static constexpr s32 k_maxDifficulty = 75;

static f32 s_entitySpawnCooldown;
static f32 s_entitySpawnTimer;
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

        s_entitySpawnTimer += dt;
        if(s_entitySpawnTimer >= 0.017f)
        {
            s_entitySpawnTimer -= 0.017f;
            if(RandomS32(0,1000) <= (s_difficulty * s_boostMultiplier))
                SpawnAsteroid();
        }
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
        if(s.type == SmokeType_Thruster || s.type == SmokeType_Blood) s.scale *= 1.0f;
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
        imm::DrawTexture("smoke", s.pos.x, s.pos.y, s.scale,s.scale, csm::ToRad(s.angle), imm::Flip_None, NULL, &clip);
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
    thruster = "thruster";
    switch(s_rocket.costume)
    {
        case(Costume_Meat): thruster = "squirt"; break;
        case(Costume_Doodle): thruster = "mouth0"; break;
        case(Costume_Rainbow): thruster = "sparkle"; break;
        case(Costume_Glitch): thruster = "static"; break;
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
    s_rocket.score = 0;
    s_rocket.frame = 0;
    s_rocket.dead  = true;
    s_rocket.collider = { Vec2(0,-8), 8.0f };
    s_rocket.collector = { Vec2(0,-8), 40.0f };
    s_rocket.costume = Costume_Red;
    s_rocket.thruster = sfx::k_invalidSoundRef;
    s_rocket.random = (s_rocket.costume == Costume_Random);
    LoadGame();
}

static s32 ScoreCompare (const void* a, const void* b)
{
    u32* aa = CS_CAST(u32*,a);
    u32* bb = CS_CAST(u32*,b);
    if(*aa > *bb) return -1;
    if(*aa < *bb) return  1;
    return 0;
}

static void HitRocket()
{
    StopThruster();

    SpawnSmoke(SmokeType_Explosion, s_rocket.pos.x, s_rocket.pos.y, RandomS32(20,40));

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
    s_rocket.dead = true;

    // Check to see if any new costumes were unlocked.
    for(s32 i=0; i<CS_ARRAY_SIZE(k_unlocks); ++i)
    {
        const Unlock& unlock = k_unlocks[i];
        if(s_rocket.score >= unlock.score)
        {
            if(!s_rocket.unlocks[unlock.costume]) // Not already unlocked.
            {
                s_rocket.unlocks[unlock.costume] = true;
                s_gameOverUnlocks.push(unlock.costume);
            }
        }
    }

    // Determine if the new score is a highscore and then save.
    if (s_rocket.score > s_rocket.highscores[9])
    {
        s_rocket.highscores[9] = s_rocket.score;
        qsort(s_rocket.highscores,CS_ARRAY_SIZE(s_rocket.highscores),sizeof(s_rocket.highscores[0]),ScoreCompare);
    }
    SaveGame();

    GoToGameOverMenu();
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
    if(s_rocket.frame >= 5)
    {
        s_rocket.frame = 0;
    }

    if(!s_rocket.dead)
    {
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
            if(abs((s_currMouseX - s_prevMouseX) * dt) >= 35.0f * dt)
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
            for(auto& asteroid: s_asteroids)
            {
                if(CheckCollision(s_rocket.pos, s_rocket.collider, asteroid.pos, asteroid.collider))
                {
                    HitRocket();
                    return;
                }
            }
        }

        // Increment the score.
        if((s_gameState == GameState_Game) && !s_gameResetting)
        {
            u32 oldScore = s_rocket.score;
            s_rocket.score += CS_CAST(s32, (2.0f * s_boostMultiplier));
            if(s_rocket.score > 999999)
                s_rocket.score = 999999;
            if(s_rocket.highscores[0] != 0 && oldScore <= s_rocket.highscores[0] && s_rocket.score > s_rocket.highscores[0])
                sfx::PlaySound("highscore");
        }
    }
}

static void RenderRocket(f32 dt)
{
    if(s_gameState == GameState_Game || s_gamePaused)
    {
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
                    imm::DrawTexture("explosion", s_rocket.pos.x-20, s_rocket.pos.y-10, 0.5f,0.5f, 0.0f, imm::Flip_None, NULL, &clip);
                    imm::DrawTexture("explosion", s_rocket.pos.x+10, s_rocket.pos.y+30, 0.5f,0.5f, 0.0f, imm::Flip_None, NULL, &clip);
                }
            }
        }
        else
        {
            // Draw the rocket.
            Rect clip = { 48*CS_CAST(f32,s_rocket.frame), 96*CS_CAST(f32,s_rocket.costume), 48, 96 };
            f32 angle = csm::ToRad(s_rocket.angle + s_rocket.shake);
            imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, imm::Flip_None, NULL, &clip);

            // Draw the score.
            bool beatHighscore = ((s_rocket.score > s_rocket.highscores[0]) && (s_rocket.highscores[0] != 0));
            BitmapFont* font = (beatHighscore) ? &s_font1 : &s_font0;
            std::string scoreStr = std::to_string(s_rocket.score);
            f32 textWidth = GetTextLineWidth(*font, scoreStr);
            if(beatHighscore) scoreStr += "!";
            f32 screenWidth = gfx::GetScreenWidth();
            f32 screenHeight = gfx::GetScreenHeight();
            DrawBitmapFont(*font, roundf((screenWidth-textWidth)*0.5f),4.0f, scoreStr);
        }
    }
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
            s_rocket.pos = Vec2(screenW*0.5f, screenH-32.0f);
            s_rocket.vel = Vec2(0);
            s_rocket.score = 0;
            s_rocket.timer = 0.0f;
            s_rocket.dead = false;
            s_entitySpawnCooldown = k_entitySpawnCooldownTime;
            s_entitySpawnTimer = 0.0f;
            s_boostMultiplier = 1.0f;
            s_difficultyTimer = 0.0f;
            s_difficulty = 50;
            s_asteroids.clear();
            s_smoke.clear();
            s_gamePaused = false;
            s_fadeOut = false;
            if(s_rocket.random)
            {
                // Pick a random costume.
                Costume costume = Costume_Random;
                while((!s_rocket.unlocks[costume]) || (costume == Costume_Random) || (costume == s_rocket.costume))
                    costume = CS_CAST(Costume, RandomS32(Costume_Red,Costume_Glitch));
                s_rocket.costume = costume;
            }
            if(s_gameState == GameState_Game)
            {
                StartThruster();
            }
            else if(s_gameState == GameState_MainMenu)
            {
                GoToMainMenu();
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
    if(s_gameState != GameState_Game || s_gamePaused || s_rocket.dead)
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
    bool leftPressed = IsMouseButtonPressed(MouseButton_Left);
    bool rightPressed = IsMouseButtonPressed(MouseButton_Right);
    if(leftPressed || rightPressed)
    {
        for(size_t i=0; i<count; ++i)
        {
            MenuOption& option = options[i];
            if(option.selected)
            {
                if(option.type == MenuOptionType_Button || option.type == MenuOptionType_Toggle)
                {
                    if(leftPressed)
                    {
                        sfx::PlaySound("select");
                        option.scale = 2.0f;
                        option.toggle = !option.toggle;
                        if(option.action)
                            option.action(option);
                    }
                }
                else if(option.type == MenuOptionType_Slider)
                {
                    sfx::PlaySound("select");
                    option.scale = 2.0f;
                    if(leftPressed)
                    {
                        option.slider += 0.1f;
                        if(option.slider >= 1.1f)
                            option.slider = 0.0f;
                    }
                    if(rightPressed)
                    {
                        option.slider -= 0.1f;
                        if(option.slider <= -0.1f)
                            option.slider = 1.0f;
                    }
                    option.slider = csm::Clamp(option.slider, 0.0f, 1.0f);
                    if(option.action)
                        option.action(option);
                }
            }
        }
    }
}

static void RenderMenuOption(MenuOption& option, f32 currAngle)
{
    f32 xPos  = option.bounds.x + (option.bounds.w * 0.5f);
    f32 yPos  = option.bounds.y + (option.bounds.h * 0.5f);
    f32 scale = option.scale;
    f32 angle = 0.0f;
    Rect clip = option.clip;
    if(option.selected)
    {
        clip.x += 128.0f;
        angle = -currAngle;
    }
    if(option.type == MenuOptionType_Toggle)
    {
        if(!option.toggle)
            clip.y += 24.0f;
    }
    if(option.type == MenuOptionType_Slider)
    {
        clip.y += (clip.h * roundf((option.slider*100.0f)/10.0f));
    }
    imm::DrawTexture("menu", xPos,yPos, scale,scale, csm::ToRad(angle), imm::Flip_None, NULL, &clip);
}

static void RenderMenuOptions(MenuOption* options, size_t count, f32 dt)
{
    static f32 s_angle = 0.0f;
    static f32 s_timer = 0.0f;

    f32 halfW = gfx::GetScreenWidth() * 0.5f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    for(size_t i=0; i<count; ++i)
        if(!options[i].selected)
            RenderMenuOption(options[i], s_angle);
    for(size_t i=0; i<count; ++i)
        if(options[i].selected)
            RenderMenuOption(options[i], s_angle);
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
    s_entitySpawnTimer = 0.0f;
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

    imm::DrawTexture("menu", halfW,48.0f, s_scaleX,s_scaleY, csm::ToRad(s_angle), imm::Flip_None, NULL, &titleClip);
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

    // Draw the highscores.
    f32 screenWidth = gfx::GetScreenWidth();
    f32 screenHeight = gfx::GetScreenHeight();
    f32 yPos = 42.0f;
    for(s32 i=0; i<10; ++i)
    {
        BitmapFont* font = (i == 0) ? &s_font1 : &s_font0;
        u32 score = s_rocket.highscores[i];
        std::string scoreStr = std::to_string(score);
        f32 textWidth = GetTextLineWidth(*font, scoreStr);
        DrawBitmapFont(*font, roundf((screenWidth-textWidth)*0.5f),yPos, scoreStr);
        yPos += 24.0f;
    }
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

    imm::DrawTexture("costume", halfW,halfH, s_costumeScale,s_costumeScale, 0.0f, imm::Flip_None, NULL, &costumeClip);
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

static s32 s_resetSaveCounter;

static void SettingsMenuActionSound(MenuOption& option)
{
    sfx::SetSoundVolume(option.slider);
}

static void SettingsMenuActionMusic(MenuOption& option)
{
    sfx::SetMusicVolume(option.slider);
}

static void SettingsMenuActionFullscreen(MenuOption& option)
{
    FullscreenWindow(option.toggle);
}

static void SettingsMenuActionVSync(MenuOption& option)
{
    EnableVSync(option.toggle);
}

static void SettingsMenuActionResetSave(MenuOption& option)
{
    s_resetSaveCounter++;
    if(s_resetSaveCounter >= 3)
    {
        s_resetSaveCounter = 0;
        sfx::PlaySound("reset");
        ResetSave();
    }
}

static void SettingsMenuActionBack(MenuOption& option)
{
    if(s_gamePaused) GoToPauseMenu();
    else GoToMainMenu();
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
{ SettingsMenuActionSound,      MenuOptionType_Slider, { 0.0f,128.0f,      180.0f,24.0f }, { 0,1128,128,24 } },
{ SettingsMenuActionMusic,      MenuOptionType_Slider, { 0.0f,128.0f+24.0f,180.0f,24.0f }, { 0,1392,128,24 } },
{ SettingsMenuActionFullscreen, MenuOptionType_Toggle, { 0.0f,128.0f+48.0f,180.0f,24.0f }, { 0, 408,128,24 } },
{ SettingsMenuActionVSync,      MenuOptionType_Toggle, { 0.0f,128.0f+72.0f,180.0f,24.0f }, { 0, 456,128,24 } },
{ SettingsMenuActionResetSave,  MenuOptionType_Button, { 0.0f,128.0f+96.0f,180.0f,24.0f }, { 0, 504,128,24 } },
{ SettingsMenuActionBack,       MenuOptionType_Button, { 0.0f,288.0f,      180.0f,24.0f }, { 0, 576,128,24 } }
};

static void UpdateSettingsMenu(f32 dt)
{
    if(s_gameState != GameState_SettingsMenu) return;
    s_settingsMenuOptions[SettingsMenuOption_Sound].slider = sfx::GetSoundVolume();
    s_settingsMenuOptions[SettingsMenuOption_Music].slider = sfx::GetMusicVolume();
    s_settingsMenuOptions[SettingsMenuOption_Fullscreen].toggle = IsFullscreen();
    s_settingsMenuOptions[SettingsMenuOption_VSync].toggle = IsVSyncOn();
    s_settingsMenuOptions[SettingsMenuOption_ResetSave].clip.y = 504+(CS_CAST(f32,s_resetSaveCounter)*24.0f);
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
    s_resetSaveCounter = 0;
}

//
// Game Over Menu
//

static void GameOverMenuActionRetry(MenuOption& option)
{
    ResetGame(GameState_Game);
}

static void GameOverMenuActionMenu(MenuOption& option)
{
    ResetGame(GameState_MainMenu);
}

enum GameOverMenuOption
{
    GameOverMenuOption_Retry,
    GameOverMenuOption_Menu,
    GameOverMenuOption_TOTAL
};

static MenuOption s_gameOverMenuOptions[GameOverMenuOption_TOTAL]
{
{ GameOverMenuActionRetry, MenuOptionType_Button, { 0.0f,248.0f,180.0f,24.0f }, { 0,1656,128,24 } },
{ GameOverMenuActionMenu,  MenuOptionType_Button, { 0.0f,272.0f,180.0f,24.0f }, { 0, 336,128,24 } }
};

static void UpdateGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;
    UpdateMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL, dt);
    if(IsKeyPressed(KeyCode_R))
        GameOverMenuActionRetry(s_gameOverMenuOptions[GameOverMenuOption_Retry]);
}

static void RenderGameOverMenu(f32 dt)
{
    if(s_gameState != GameState_Game) return;
    if(s_gameResetting || !s_rocket.dead) return;
    RenderMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL, dt);
    Rect titleClip = { 0,1704,256,32 };
    imm::DrawTexture("menu", gfx::GetScreenWidth()*0.5f,24.0f, &titleClip);

    // Draw the score achieved.
    bool newHighscore = (s_rocket.score >= s_rocket.highscores[0]);
    std::string scoreStr = std::to_string(s_rocket.score);
    f32 textWidth = GetTextLineWidth(s_bigFont0, scoreStr);
    if(newHighscore) scoreStr += "!";
    f32 screenWidth = gfx::GetScreenWidth();
    f32 screenHeight = gfx::GetScreenHeight();
    f32 textHeight = screenHeight*0.33f;
    DrawBitmapFont(s_bigFont0, roundf((screenWidth-textWidth)*0.5f),textHeight, scoreStr);

    // Draw a message depending on what the score was.
    Rect newHighClip  = { 0,1680,256,24 };
    Rect wellDoneClip = { 0,1760,256,24 };
    Rect niceTryClip  = { 0,1736,256,24 };

    Rect clip = niceTryClip;
    if(s_rocket.score >= s_rocket.highscores[9]) clip = wellDoneClip;
    if(s_rocket.score >= s_rocket.highscores[0]) clip = newHighClip;

    imm::DrawTexture("menu", screenWidth*0.5f,textHeight+64.0f, &clip);
}

static void GoToGameOverMenu()
{
    s_gameState = GameState_Game;
    ResetMenuOptions(s_gameOverMenuOptions, GameOverMenuOption_TOTAL);
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

static void PauseMenuActionSettings(MenuOption& option)
{
    GoToSettingsMenu();
}

static void PauseMenuActionMenu(MenuOption& option)
{
    ResetGame(GameState_MainMenu);
    sfx::ResumeMusic();
}

enum PauseMenuOption
{
    PauseMenuOption_Resume,
    PauseMenuOption_Settings,
    PauseMenuOption_Menu,
    PauseMenuOption_TOTAL
};

static MenuOption s_pauseMenuOptions[PauseMenuOption_TOTAL]
{
{ PauseMenuActionResume,   MenuOptionType_Button, { 0.0f,200.0f,180.0f,24.0f }, { 0,312,128,24 } },
{ MainMenuActionSettings,  MenuOptionType_Button, { 0.0f,224.0f,180.0f,24.0f }, { 0,264,128,24 } },
{ PauseMenuActionMenu,     MenuOptionType_Button, { 0.0f,248.0f,180.0f,24.0f }, { 0,336,128,24 } }
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
    if(!s_gamePaused) return;

    f32 screenW = gfx::GetScreenWidth();
    f32 screenH = gfx::GetScreenHeight();
    f32 halfW   = screenW * 0.5f;
    f32 halfH   = screenH * 0.5f;

    imm::DrawRectFilled(0,0,screenW,screenH, Vec4(0,0,0,0.5f));

    if(s_gameState != GameState_Game) return;

    static Rect s_pauseClip = { 0,160,256,32 };

    imm::DrawTexture("menu", halfW,halfH-40, &s_pauseClip);
    RenderMenuOptions(s_pauseMenuOptions, PauseMenuOption_TOTAL, dt);
}

static void GoToPauseMenu()
{
    s_gameState = GameState_Game;
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

        s_boostMultiplier = 1.0f;
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
