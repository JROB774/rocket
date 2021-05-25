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

//
// Game
//

enum GameState
{
    GameState_Menu,
    GameState_Game
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
    Costume_TOTAL
};

struct Rocket
{
    Vec2 pos;
    Vec2 vel;
    f32 angle;
    f32 shake;
    f32 timer;
    s32 score;
    s32 frame;
    bool dead;
    Collider collider;
    sfx::SoundRef thruster;
    Costume costume;
};

static GameState s_gameState;
static bool s_gamePaused;
static bool s_gameResetting;
static s32 s_highScore;
static u32 s_gameFrame;
static Rocket s_rocket;

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
// Entity Spawn
//

enum EntityType
{
    EntityType_Asteroid,
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
        f32 scale = (s.type == SmokeType_Small || s.type == SmokeType_SmallStationary) ? 0.5f : 1.0f;
        imm::DrawTexture("smoke", s.pos.x, s.pos.y, scale,scale, csm::ToRad(s.angle), imm::Flip_None, &clip);
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
    std::string thruster = "thruster";
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
    s_rocket.pos.x = (gfx::GetScreenWidth()*0.5f);
    s_rocket.pos.y = gfx::GetScreenHeight() - 32.0f;
    s_rocket.vel   = Vec2(0);
    s_rocket.angle = 0.0f;
    s_rocket.shake = 0.0f;
    s_rocket.timer = 1000.0f; // Stop the explosion on start.
    s_rocket.score = 0;
    s_rocket.frame = 0;
    s_rocket.dead  = true;
    s_rocket.collider = { Vec2(0,-8), 8.0f };
    s_rocket.costume = Costume_Red;
    s_rocket.thruster = sfx::k_invalidSoundRef;
}

static void HitRocket()
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
    s_rocket.dead = true;
    s_rocket.timer = 0.0f;
}

static void UpdateRocket(f32 dt)
{
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
            SpawnSmoke(smokeType, s_rocket.pos.x+RandomF32(-3.0f,3.0f), s_rocket.pos.y+20.0f, smokeCount);
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
        if((s_gameState == GameState_Game) && !s_gameResetting)
        {
            s32 oldScore = s_rocket.score;
            s_rocket.score += 2;
            if(s_highScore != 0 && oldScore <= s_highScore && s_rocket.score > s_highScore)
                sfx::PlaySound("highscore");
        }
    }
}

static void RenderRocket(f32 dt)
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
                imm::DrawTexture("explosion", s_rocket.pos.x-20, s_rocket.pos.y-10, 0.5f,0.5f, 0.0f, imm::Flip_None, &clip);
                imm::DrawTexture("explosion", s_rocket.pos.x+10, s_rocket.pos.y+30, 0.5f,0.5f, 0.0f, imm::Flip_None, &clip);
            }
        }
    }
    else
    {
        // Draw the rocket.
        Rect clip = { 96+(48*CS_CAST(f32,s_rocket.frame)), 96*CS_CAST(f32,s_rocket.costume), 48, 96 };
        f32 angle = csm::ToRad(s_rocket.angle + s_rocket.shake);
        imm::DrawTexture("rocket", s_rocket.pos.x, s_rocket.pos.y, 1.0f, 1.0f, angle, imm::Flip_None, &clip);
    }

    // Draw the score.
    if(s_gameState == GameState_Game)
    {
        bool beatHighScore = ((s_rocket.score > s_highScore) && (s_highScore != 0));
        BitmapFont* font = (beatHighScore) ? &s_font1 : &s_font0;
        std::string scoreStr = std::to_string(s_rocket.score);
        f32 textWidth = GetTextLineWidth(*font, scoreStr);
        if(beatHighScore) scoreStr += "!";
        f32 screenWidth = gfx::GetScreenWidth();
        f32 screenHeight = gfx::GetScreenHeight();
        DrawBitmapFont(*font, (screenWidth-textWidth)*0.5f,4.0f, scoreStr);
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
// Transition
//

static f32 s_fadeHeight = 0.0f;
static bool s_fadeOut = false;

static void ResetGame()
{
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
            s_gameState = GameState_Game;
            if(s_rocket.score > s_highScore)
                s_highScore = s_rocket.score;
            s_rocket.pos.x = (screenW*0.5f);
            s_rocket.pos.y = screenH - 32.0f;
            s_rocket.dead = false;
            s_rocket.score = 0;
            s_rocket.timer = 0.0f;
            s_entitySpawnCooldown = 1.0f;
            s_asteroids.clear();
            s_smoke.clear();
            s_fadeOut = false;
            StartThruster();
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
        s_backOffset[i] += s_backSpeed[i] * dt;
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
    if(s_gameState == GameState_Menu || s_gamePaused)
    {
        Vec2 pos = GetScreenMousePos();
        f32 x = csm::Clamp(roundf(pos.x), 0.0f,gfx::GetScreenWidth());
        f32 y = csm::Clamp(roundf(pos.y), 0.0f,gfx::GetScreenHeight());
        imm::DrawTexture("cursor", x,y);
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
            s_entitySpawnCooldown = 1.0f;
            ResetGame();
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

        Rect titleClip = { 0,0,256,64 };
        imm::DrawTexture("menu", gfx::GetScreenWidth()*0.5f,48.0f, s_scaleX,s_scaleY, csm::ToRad(s_angle), imm::Flip_None, &titleClip);
    }
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

        sfx::SetSoundVolume(0.4f);
        sfx::SetMusicVolume(0.4f);

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

        s_gameState = GameState_Menu;
        s_gamePaused = false;
    }

    void Quit()
    {
        // Nothing...
    }

    void Update(f32 dt)
    {
        // Handle locking/unlocking and showing/hiding the mouse with debug mode.
        static bool s_lockMouse = true;
        static bool s_showMouse = false;
        if(IsDebugMode())
        {
            if(IsKeyPressed(KeyCode_Space))
                s_lockMouse = !s_lockMouse;
            s_showMouse = true;
        }
        else
        {
            s_lockMouse = (!s_gamePaused && (s_gameState != GameState_Menu));
            s_showMouse = false;
        }
        LockMouse(s_lockMouse);
        ShowCursor(s_showMouse);

        if((s_gameState == GameState_Game) && !s_gameResetting && !s_rocket.dead)
        {
            if(IsKeyPressed(KeyCode_Escape))
            {
                s_gamePaused = !s_gamePaused;
                sfx::PlaySound("pause");

                if(s_gamePaused)
                {
                    sfx::PauseMusic();
                    StopThruster();
                }
                else
                {
                    sfx::ResumeMusic();
                    StartThruster();
                }
            }
        }

        if(!s_gamePaused)
        {
            // If the rocket is dead then pressing R resets the game.
            if(s_gameState == GameState_Game && s_rocket.dead && !s_gameResetting)
                if(IsMouseButtonPressed(MouseButton_Left))
                    ResetGame();

            if((s_gameState == GameState_Game) && !s_gameResetting)
                MaybeSpawnEntity(dt);
            UpdateBackground(dt);
            UpdateAsteroids(dt);
            UpdateSmoke(dt);
            UpdateRocket(dt);
            UpdateMenu(dt);
        }

        s_gameFrame++;
    }

    void Render(f32 dt)
    {
        RenderBackground(dt);
        RenderSmoke(dt);
        RenderAsteroids(dt);
        RenderRocket(dt);
        RenderMenu(dt);
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
    appConfig.window.min = Vec2i(180,320);
    appConfig.screenSize = Vec2i(180,320);
    appConfig.app = Allocate<RocketApp>(CS_MEM_GAME);
    return appConfig;
}
