static void LoadBitmapFont(BitmapFont& font, f32 cw, f32 ch, std::string texture)
{
    font.texture = texture;
    font.charWidth = cw;
    font.charHeight = ch;

    for(s32 iy=0; iy<3; ++iy)
        for(s32 ix=0; ix<32; ++ix)
            font.bounds[iy*32+ix] = { CS_CAST(f32,ix)*cw, CS_CAST(f32,iy)*ch, cw, ch };
}

static f32 GetCharWidth(BitmapFont& font, char c)
{
    if(font.charWidth == 14) // Small font.
    {
        switch(c)
        {
            case('!'): return  6;
            case('0'): return 13;
            case('1'): return 10;
            case('2'): return 13;
            case('3'): return 13;
            case('4'): return 13;
            case('5'): return 13;
            case('6'): return 13;
            case('7'): return 13;
            case('8'): return 13;
            case('9'): return 13;
        }
    }
    if(font.charWidth == 24) // Big font.
    {
        switch(c)
        {
            case('!'): return  8;
            case('0'): return 21;
            case('1'): return 14;
            case('2'): return 21;
            case('3'): return 21;
            case('4'): return 23;
            case('5'): return 20;
            case('6'): return 21;
            case('7'): return 21;
            case('8'): return 21;
            case('9'): return 20;
        }
    }
    return font.charWidth;
}

static f32 GetTextLineWidth(BitmapFont& font, std::string text, s32 line)
{
    f32 lineWidth = 0;
    s32 lineIndex = 0;
    for(size_t i=0; i<text.length(); ++i)
    {
        if(text[i] == '\n')
        {
            if(lineIndex++ == line) return lineWidth;
            else lineWidth = 0;
        }
        else
        {
            lineWidth += GetCharWidth(font, text[i]);
        }
    }
    return lineWidth;
}

static void DrawBitmapFont(BitmapFont& font, f32 x, f32 y, std::string text, nkVec4 color)
{
    // NOTE: We just assume the caller wants multi-line text to be center aligned.

    f32 ix = x;
    f32 iy = y;

    s32 line = 0;

    nkVec2 anchor = { 0,0 };

    for(size_t i=0; i<text.length(); ++i)
    {
        if(text[i] == '\n')
        {
            ix = roundf(x + (GetTextLineWidth(font, text, 0)*0.5f) - (GetTextLineWidth(font, text, line+1)*0.5f));
            iy += font.charHeight;
            line++;
        }
        else
        {
            Rect bounds = font.bounds[CS_CAST(u8,text.at(i))];
            imm::DrawTexture(font.texture, ix,iy, 1.0f,1.0f, 0.0f, imm::Flip_None, &anchor, &bounds, color);
            ix += roundf(GetCharWidth(font, text[i]));
        }
    }
}
