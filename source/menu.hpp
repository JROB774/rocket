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
    MenuOption(MenuOptionAction iAction, MenuOptionType iType, Rect iBounds, Rect iClip)
    {
        action = iAction;
        type = iType;
        bounds = iBounds;
        clip = iClip;
        selected = false;
        scale = 1.0f;
        targetScale = 1.0f;
        toggle = false;
        slider = 1.0f;
    }

    MenuOptionAction action;
    MenuOptionType type;
    Rect bounds;
    Rect clip;
    bool selected;
    f32 scale;
    f32 targetScale;
    bool toggle;
    f32 slider;
};

static void UpdateMenuOptions(MenuOption* options, size_t count, f32 dt);
static void RenderMenuOption(MenuOption& option, f32 currAngle);
static void RenderMenuOptions(MenuOption* options, size_t count, f32 dt);
static void ResetMenuOptions(MenuOption* options, size_t count);

static void RenderCursor(f32 dt);
