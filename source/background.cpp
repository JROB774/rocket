static void CreateBackground()
{
    f32 speed = 360.0f;
    for(s32 i=k_backCount-1; i>=0; --i)
    {
        s_backSpeed[i] = speed;
        s_backOffset[i] = GetScreenHeight() * 0.5f;
        speed += 120.0f;
    }
}

static void UpdateBackground(f32 dt)
{
    f32 screenHeight = GetScreenHeight();
    for(s32 i=0; i<k_backCount; ++i)
    {
        s_backOffset[i] += s_backSpeed[i] * dt;
        if(s_backOffset[i] >= screenHeight * 1.5f)
            s_backOffset[i] -= screenHeight;
    }
}

static void RenderBackground(f32 dt)
{
    Clear(0.0f, 0.05f, 0.2f);

    f32 screenWidth = GetScreenWidth();
    f32 screenHeight = GetScreenHeight();

    Rect clip = { 0, 0, 180, 320 };
    nkVec4 color = { 1,1,1,0.4f };

    imm::BeginTextureBatch("back");
    for(s32 i=0; i<k_backCount; ++i)
    {
        imm::DrawBatchedTexture(screenWidth*0.5f,s_backOffset[i], &clip, color);
        imm::DrawBatchedTexture(screenWidth*0.5f,s_backOffset[i]-screenHeight, &clip, color);
        clip.x += 180.0f;
    }
    imm::EndTextureBatch();
}
