#pragma once

struct Rocket
{
    cs::Vec2 pos;
    cs::Vec2 vel;
    cs::f32 angle;
    cs::f32 shake;
};

static void CreateRocket(Rocket& rocket);
static void UpdateRocket(Rocket& rocket, cs::f32 dt);
static void RenderRocket(Rocket& rocket, cs::f32 dt);
