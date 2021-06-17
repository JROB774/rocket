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
