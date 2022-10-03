static bool s_wantLockCursor = true;

#ifndef __EMSCRIPTEN__
bool CanvasHasFocus()
{
    return true;
}
#else
EM_JS(bool, CanvasHasFocus, (),
{
    var canvas = document.getElementById("canvas");
    return (document.pointerLockElement === canvas);
});
#endif // __EMSCRIPTEN__

static void UpdateCursor(f32 dt)
{
    s_wantLockCursor = (!s_gamePaused && !s_rocket.dead && (s_gameState == GameState_Game));
    s_gameUnfocused = !CanvasHasFocus();

    #ifndef __EMSCRIPTEN__
    LockMouse(s_wantLockCursor);
    #else
    if(!CanvasHasFocus() && s_wantLockCursor)
        PauseGame();
    #endif // __EMSCRIPTEN__
}

static void RenderCursor(f32 dt)
{
    if(CanvasHasFocus())
    {
        if(s_gameState != GameState_Game || s_gamePaused || s_rocket.dead)
        {
            nkVec2 pos = GetScreenMousePos();
            f32 x = roundf(pos.x);
            f32 y = roundf(pos.y);
            imm::DrawTexture("cursor", x,y);
        }
    }
}

static void RenderUnfocused(f32 dt)
{
    if(s_gameUnfocused)
    {
        imm::DrawTexture("unfocused", GetWindowWidth()/4,GetWindowHeight()/4);
    }
}

static void ResetCursor()
{
    s32 halfW = GetWindowWidth() / 2;
    s32 halfH = GetWindowHeight() / 2;
    SDL_WarpMouseInWindow(s_context.window, halfW, halfH);
}
