#pragma once

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

static void CreateSmoke();
static void SpawnSmoke(SmokeType type, f32 x, f32 y, s32 count);
static void UpdateSmoke(f32 dt);
static void RenderSmoke(f32 dt);
