static void UpdateMenuOptions(MenuOption* options, size_t count, f32 dt)
{
    nkVec2 mouse = GetScreenMousePos();
    if(!CanvasHasFocus())
    {
        mouse.x = -20.0f;
        mouse.y = -20.0f;
    }

    for(size_t i=0; i<count; ++i)
    {
        MenuOption& option = options[i];
        bool oldSelected = option.selected;
        option.selected = PointInRect(mouse, option.bounds);
        option.targetScale = (option.selected) ? 1.33f : 1.0f;
        option.scale = nk::lerp(option.scale, option.targetScale, 0.5f);

        // If the option went from non-selected to selected then play a sound.
        if(option.selected && (oldSelected != option.selected))
            PlaySound("click");
    }

    // Handle the interaction logic based on what type of option it is.
    bool leftPressed = IsMouseButtonPressed(MouseButton_Left);
    bool rightPressed = IsMouseButtonPressed(MouseButton_Right);
    if(leftPressed || rightPressed)
    {
        for(size_t i=0; i<count; ++i)
        {
            MenuOption& option = options[i];
            if(option.selected)
            {
                if(option.type == MenuOptionType_Button || option.type == MenuOptionType_Toggle)
                {
                    if(leftPressed)
                    {
                        PlaySound("select");
                        option.scale = 2.0f;
                        option.toggle = !option.toggle;
                        if(option.action)
                            option.action(option);
                    }
                }
                else if(option.type == MenuOptionType_Slider)
                {
                    PlaySound("select");
                    option.scale = 2.0f;
                    if(leftPressed)
                    {
                        option.slider += 0.1f;
                        if(option.slider >= 1.1f)
                            option.slider = 0.0f;
                    }
                    if(rightPressed)
                    {
                        option.slider -= 0.1f;
                        if(option.slider <= -0.1f)
                            option.slider = 1.0f;
                    }
                    option.slider = nk::clamp(option.slider, 0.0f, 1.0f);
                    if(option.action)
                        option.action(option);
                }
            }
        }
    }
}

static void RenderMenuOption(MenuOption& option, f32 currAngle)
{
    f32 xPos  = option.bounds.x + (option.bounds.w * 0.5f);
    f32 yPos  = option.bounds.y + (option.bounds.h * 0.5f);
    f32 scale = option.scale;
    f32 angle = 0.0f;
    Rect clip = option.clip;
    if(option.selected)
    {
        clip.x += 128.0f;
        angle = -currAngle;
    }
    if(option.type == MenuOptionType_Toggle)
    {
        if(!option.toggle)
            clip.y += 24.0f;
    }
    if(option.type == MenuOptionType_Slider)
    {
        clip.y += (clip.h * roundf((option.slider*100.0f)/10.0f));
    }
    imm::DrawBatchedTexture(xPos,yPos, scale,scale, nk::torad(angle), imm::Flip_None, NULL, &clip);
}

static void RenderMenuOptions(MenuOption* options, size_t count, f32 dt)
{
    static f32 s_angle = 0.0f;
    static f32 s_timer = 0.0f;

    f32 halfW = GetScreenWidth() * 0.5f;

    s_timer += dt;
    s_angle = SinRange(-10.0f, 10.0f, s_timer*2.5f);

    imm::BeginTextureBatch("menu");
    for(size_t i=0; i<count; ++i)
        if(!options[i].selected)
            RenderMenuOption(options[i], s_angle);
    for(size_t i=0; i<count; ++i)
        if(options[i].selected)
            RenderMenuOption(options[i], s_angle);
    imm::EndTextureBatch();
}

static void ResetMenuOptions(MenuOption* options, size_t count)
{
    for(size_t i=0; i<count; ++i)
    {
        options[i].selected = false;
        options[i].targetScale = 1.0f;
        options[i].scale = 1.0f;
    }
}
