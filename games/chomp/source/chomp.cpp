#include "chomp.hpp"

static const f32 k_minChompPos = 104.0f;
static const f32 k_maxChompPos = 168.0f;
static const f32 k_chompDownVelocity = 180.0f;
static const f32 k_chompUpVelocity = 35.0f;
static const f32 k_chompMinVelocity = -325.0f;
static const f32 k_chompMaxVelocity = 870.0f;
static const f32 k_chompCooldown = 0.15f;
static const f32 k_cloudSpeed = 10.0f;

void ChompApp::Init()
{
    gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
    gfx::SetScreenFilter(gfx::Filter_Nearest);

    m_chomp.pos = Vec2(196,104);
    m_chomp.vel = 0.0f;
    m_chomp.landedCooldown = 0.0f;
    m_chomp.chomping = false;
    m_chomp.landed = false;
    m_cloudPosX = gfx::GetScreenWidth();
}
void ChompApp::Quit()
{
    // Nothing...
}
void ChompApp::Update(f32 dt)
{
    f32 screenX = CS_CAST(f32, gfx::GetScreenWidth());
    f32 screenY = CS_CAST(f32, gfx::GetScreenHeight());

    // CLOUD LOGIC

    m_cloudPosX -= k_cloudSpeed * dt;
    if(m_cloudPosX <= -screenX) // Loop around.
    {
        m_cloudPosX += screenX * 2.0f;
    }

    // CHOMP LOGIC

    // If we've landed we cooldown/freeze for a bit.
    if(m_chomp.landedCooldown > 0.0f)
    {
        m_chomp.landedCooldown -= dt;
        return;
    }
    // Once we've landed and cooled down we go straight up.
    if(m_chomp.landed)
    {
        m_chomp.chomping = false;
        m_chomp.landed = false;
    }

    // Check input.
    if(IsKeyPressed(KeyCode_Space))
    {
        m_chomp.vel += k_chompDownVelocity; // Extra boost on initial chomp.
        m_chomp.chomping = true;
        m_chomp.landed = false;
    }

    // Update velocity.
    if(m_chomp.chomping) m_chomp.vel += k_chompDownVelocity; else m_chomp.vel -= k_chompUpVelocity;
    m_chomp.vel = Clamp(m_chomp.vel, k_chompMinVelocity, k_chompMaxVelocity);

    // Update position.
    m_chomp.pos.y += m_chomp.vel * dt;
    m_chomp.pos.y = Clamp(m_chomp.pos.y, k_minChompPos, k_maxChompPos);

    // Check landed.
    if(m_chomp.pos.y >= k_maxChompPos)
    {
        m_chomp.vel = 0.0f;
        m_chomp.landed = true;
        m_chomp.landedCooldown = k_chompCooldown;
    }
}
void ChompApp::Render(f32 dt)
{
    f32 screenX = CS_CAST(f32, gfx::GetScreenWidth());
    f32 screenY = CS_CAST(f32, gfx::GetScreenHeight());
    f32 halfX = screenX / 2.0f;
    f32 halfY = screenY / 2.0f;

    f32 cloudX = roundf(m_cloudPosX);
    f32 cloudY = halfY;
    f32 chompX = roundf(m_chomp.pos.x);
    f32 chompY = roundf(m_chomp.pos.y);

    gfx::Clear(RGBAToVec4(61,63,191));

    imm::DrawTexture("background", halfX,halfY);
    imm::DrawTexture("clouds", cloudX,cloudY);
    imm::DrawTexture("clouds", cloudX+(screenX*2.0f),halfY);
    imm::DrawTexture("chomp", chompX,chompY);
    imm::DrawTexture("foreground", halfX,halfY);
}

AppConfig csMain(int argc, char** argv)
{
    AppConfig appConfig;
    appConfig.title = "Chomp";
    appConfig.window.min = Vec2i(320,180);
    appConfig.screenSize = Vec2i(320,180);
    appConfig.app = Allocate<ChompApp>(CS_MEM_GAME);
    return appConfig;
}
