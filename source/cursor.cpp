static bool s_wantLockCursor = true;

#ifndef __EMSCRIPTEN__
bool CanvasHasFocus()
{
    u32 windowFlags = SDL_GetWindowFlags(s_context.window);
    return NK_CHECK_FLAGS(windowFlags, SDL_WINDOW_INPUT_FOCUS);
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
    s_gameUnfocused = ((s_gameState == GameState_Game) && !CanvasHasFocus());

    #ifndef __EMSCRIPTEN__
    LockMouse(s_wantLockCursor);
    #endif // __EMSCRIPTEN__

    // Pause the game if we've lost focus.
    if(s_gameUnfocused && !s_gamePaused)
    {
        PauseGame();
    }
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
