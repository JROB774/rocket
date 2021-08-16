#pragma once

static constexpr s32 k_backCount = 3;

static f32 s_backSpeed[k_backCount];
static f32 s_backOffset[k_backCount];

static void CreateBackground();
static void UpdateBackground(f32 dt);
static void RenderBackground(f32 dt);
