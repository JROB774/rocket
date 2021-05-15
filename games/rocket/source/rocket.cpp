static constexpr cs::f32 k_rocketVelocityMultiplier = 25.0f;
static constexpr cs::f32 k_rocketTerminalVelocity = 9.5;
static constexpr cs::f32 k_rocketMaxAngle = 25.0f;
static constexpr cs::f32 k_rocketMaxShake = 2.0f;

static void CreateRocket(Rocket& rocket)
{
    cs::gfx::Texture texture = *cs::GetAsset<cs::gfx::Texture>("rocket");
    cs::gfx::SetTextureFilter(texture, cs::gfx::Filter_Nearest);

    rocket.pos.x = (cs::gfx::GetScreenWidth()/2.0f);
    rocket.pos.y = cs::gfx::GetScreenHeight() - 48.0f;
    rocket.vel   = cs::Vec2(0);
    rocket.angle = 0.0f;
    rocket.shake = 0.0f;
}

static void UpdateRocket(Rocket& rocket, cs::f32 dt)
{
    rocket.vel.x += cs::GetRelativeMousePos().x / 10.0f;
    rocket.vel.y += cs::GetRelativeMousePos().y / 20.0f;

    rocket.angle = cs::Clamp(rocket.vel.x, -k_rocketMaxAngle, k_rocketMaxAngle);
    rocket.shake = cs::RandomF32(-k_rocketMaxShake, k_rocketMaxShake);

    rocket.vel.x = cs::Clamp(rocket.vel.x, -(k_rocketTerminalVelocity*1.5f), (k_rocketTerminalVelocity*1.5f));
    rocket.vel.y = cs::Clamp(rocket.vel.y, -k_rocketTerminalVelocity, k_rocketTerminalVelocity);

    rocket.pos += (rocket.vel * k_rocketVelocityMultiplier) * dt;
    rocket.pos.x = cs::Clamp(rocket.pos.x, 0.0f, cs::gfx::GetScreenWidth());
    rocket.pos.y = cs::Clamp(rocket.pos.y, 0.0f, cs::gfx::GetScreenHeight());

    rocket.vel = cs::Lerp(rocket.vel, cs::Vec2(0), cs::Vec2(0.1f));
}

static void RenderRocket(Rocket& rocket, cs::f32 dt)
{
    static cs::Rect s_clip = { 48, 0, 48, 96 };
    cs::f32 angle = cs::ToRad(rocket.angle + rocket.shake);
    cs::imm::DrawTexture("rocket", rocket.pos.x, rocket.pos.y, 1.0f, 1.0f, angle, cs::imm::Flip_None, &s_clip);
    s_clip.x += 48.0f;
    if(s_clip.x >= 288.0f) s_clip.x = 48.0f;
}
