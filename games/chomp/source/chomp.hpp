#pragma once

#include "cs_utility.hpp"
#include "cs_math.hpp"
#include "cs_memory.hpp"
#include "cs_assets.hpp"
#include "cs_application.hpp"
#include "cs_platform.hpp"
#include "cs_debug.hpp"
#include "cs_graphics.hpp"
#include "cs_font.hpp"
#include "cs_audio.hpp"

using namespace cs;

struct Chomp
{
    Vec2 pos;
    f32 vel;
    f32 landedCooldown;
    bool chomping;
    bool landed;
};

class ChompApp: public Application
{
public:
    Chomp m_chomp;
    f32 m_cloudPosX;

    void Init() override;
    void Quit() override;
    void Update(f32 dt) override;
    void Render(f32 dt) override;
};
