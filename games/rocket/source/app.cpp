void RocketApp::Init()
{
    cs::gfx::SetScreenScaleMode(cs::gfx::ScaleMode_Pixel);
    cs::gfx::SetScreenFilter(cs::gfx::Filter_Nearest);

    CreateRocket(m_rocket);
}

void RocketApp::Quit()
{
    // Nothing...
}

void RocketApp::Update(cs::f32 dt)
{
    cs::LockMouse(!cs::IsDebugMode());
    UpdateRocket(m_rocket, dt);
}

void RocketApp::Render(cs::f32 dt)
{
    cs::gfx::Clear(0.0f, 0.05f, 0.2f);
    RenderRocket(m_rocket, dt);
}

void RocketApp::DebugRender(cs::f32 dt)
{
    // Nothing...
}
