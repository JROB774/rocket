#pragma once

class RocketApp: public Application
{
public:
    void Init();
    void Quit();
    void Update(f32 dt);
    void Render(f32 dt);
    void DebugRender(f32 dt);
};
