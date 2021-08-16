#pragma once

struct MenuOption;

typedef void(*MenuOptionAction)(MenuOption& option);

enum MenuOptionType
{
    MenuOptionType_Button,
    MenuOptionType_Toggle,
    MenuOptionType_Slider
};

struct MenuOption
{
    MenuOptionAction action = NULL;
    MenuOptionType type = MenuOptionType_Button;
    Rect bounds = {};
    Rect clip = {};
    bool selected = false;
    f32 scale = 1.0f;
    f32 targetScale = 1.0f;
    bool toggle = false;
    f32 slider = 1.0f;
};

static void UpdateMenuOptions(MenuOption* options, size_t count, f32 dt);
static void RenderMenuOption(MenuOption& option, f32 currAngle);
static void RenderMenuOptions(MenuOption* options, size_t count, f32 dt);
static void ResetMenuOptions(MenuOption* options, size_t count);

static void RenderCursor(f32 dt);
