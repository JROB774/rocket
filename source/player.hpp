#pragma once

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

struct Rocket
{
    nkVec2 pos;
    nkVec2 vel;
    f32 angle;
    f32 shake;
    f32 timer;
    u32 score;
    s32 frame;
    bool dead;
    Collider collider;
    Collider collector;
    SoundRef thruster;
    bool random;
    // Save data.
    Costume costume;
    bool unlocks[Costume_TOTAL];
    u32 highscores[10];
};

struct Unlock
{
    Costume costume;
    u32 score;
};

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

static constexpr f32 k_rocketVelocityMultiplier = 25.0f;
static constexpr f32 k_rocketTerminalVelocity = 9.5f;
static constexpr f32 k_rocketMaxAngle = 25.0f;
static constexpr f32 k_rocketMaxShake = 2.0f;

static Rocket s_rocket;
static Costume s_currentCostume;

static void StartThruster();
static void StopThruster();
static void CreateRocket();
static void HitRocket();
static void UpdateRocket(f32 dt);
static void RenderRocket(f32 dt);
