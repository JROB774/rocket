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

// Game headers and sources...

class ChompApp: public Application
{
public:
    Vec2 m_chompPos;

    void Init() override;
    void Quit() override;
    void Update(f32 dt) override;
    void Render(f32 dt) override;
};
