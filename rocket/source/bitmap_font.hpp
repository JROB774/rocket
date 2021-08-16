#pragma once

struct BitmapFont
{
    Rect bounds[256];
    std::string texture;
    f32 charWidth;
    f32 charHeight;
};

static BitmapFont s_font0;
static BitmapFont s_font1;
static BitmapFont s_bigFont0;
static BitmapFont s_bigFont1;

static void LoadBitmapFont(BitmapFont& font, f32 cw, f32 ch, std::string texture);
static f32 GetCharWidth(BitmapFont& font, char c);
static f32 GetTextLineWidth(BitmapFont& font, std::string text, s32 line = 0);
static void DrawBitmapFont(BitmapFont& font, f32 x, f32 y, std::string text, Vec4 color = Vec4(1));
