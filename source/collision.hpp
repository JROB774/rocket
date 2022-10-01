#pragma once

struct Collider
{
    nkVec2 offset;
    f32 radius;
};

static bool CheckCollision(nkVec2 aPos, const Collider& a, nkVec2 bPos, const Collider& b);
static bool PointInRect(nkVec2 p, Rect r);
