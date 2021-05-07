#include "chomp.hpp"

static const f32 k_minChompPos = 104.0f;
static const f32 k_maxChompPos = 168.0f;
static const f32 k_chompDownSpeed = 640.0f;

void ChompApp::Init()
{
    gfx::SetScreenScaleMode(gfx::ScaleMode_Pixel);
    gfx::SetScreenFilter(gfx::Filter_Nearest);

    m_chompPos = Vec2(196,104);
}
void ChompApp::Quit()
{
    // Nothing...
}
void ChompApp::Update(f32 dt)
{
    if(IsKeyDown(KeyCode_Space)) m_chompPos.y += (k_chompDownSpeed * dt);
    else m_chompPos.y -= ((k_chompDownSpeed*0.75f) * dt);
    m_chompPos.y = Clamp(m_chompPos.y, k_minChompPos,k_maxChompPos);
}
void ChompApp::Render(f32 dt)
{
    f32 halfX = CS_CAST(f32, gfx::GetScreenWidth()) / 2.0f;
    f32 halfY = CS_CAST(f32, gfx::GetScreenHeight()) / 2.0f;

    gfx::Clear(RGBAToVec4(61,63,191));

    imm::DrawTexture("background", halfX,halfY);
    imm::DrawTexture("chomp", m_chompPos.x,m_chompPos.y);
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
