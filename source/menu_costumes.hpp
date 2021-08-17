#pragma once

enum CostumesMenuOption
{
    CostumesMenuOption_Left,
    CostumesMenuOption_Right,
    CostumesMenuOption_Back,
    CostumesMenuOption_TOTAL
};

static constexpr f32 k_costumeLockedTextOffset = 192.0f;
static f32 s_costumeScale = 1.0f;

static void UpdateCostumesMenu(f32 dt);
static void RenderCostumesMenu(f32 dt);
static void GoToCostumesMenu();
