#pragma once

class RocketApp: public cs::Application
{
public:
    Rocket m_rocket;

    void Init();
    void Quit();
    void Update(cs::f32 dt);
    void Render(cs::f32 dt);
    void DebugRender(cs::f32 dt);
};
