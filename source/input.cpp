static constexpr s16 k_gamepadStickDeadzone = 12000;

//
// Keyboard
//

static bool IsKeyDown(KeyCode code)
{
    if(code == KeyCode_Invalid) return false;
    return (GetInput().currentKeyState[code] != 0);
}

static bool IsKeyUp(KeyCode code)
{
    if(code == KeyCode_Invalid) return false;
    return (GetInput().currentKeyState[code] == 0);
}

static bool IsKeyPressed(KeyCode code)
{
    if(code == KeyCode_Invalid) return false;
    return (GetInput().currentKeyState[code] != 0 &&
            GetInput().previousKeyState[code] == 0);
}

static bool IsKeyReleased(KeyCode code)
{
    if(code == KeyCode_Invalid) return false;
    return (GetInput().currentKeyState[code] == 0 &&
            GetInput().previousKeyState[code] != 0);
}

static bool IsAnyKeyDown()
{
    for(s32 i=0; i<NK_CAST(s32, KeyCode_TOTAL); ++i)
        if(IsKeyDown(NK_CAST(KeyCode, i))) return true;
    return false;
}

static bool IsAnyKeyUp()
{
    for(s32 i=0; i<NK_CAST(s32, KeyCode_TOTAL); ++i)
        if(IsKeyUp(NK_CAST(KeyCode, i))) return true;
    return false;
}

static bool IsAnyKeyPressed()
{
    for(s32 i=0; i<NK_CAST(s32, KeyCode_TOTAL); ++i)
        if(IsKeyPressed(NK_CAST(KeyCode, i))) return true;
    return false;
}

static bool IsAnyKeyReleased()
{
    for(s32 i=0; i<NK_CAST(s32, KeyCode_TOTAL); ++i)
        if(IsKeyReleased(NK_CAST(KeyCode, i))) return true;
    return false;
}

//
// Mouse
//

static s32 GetMouseScrollHorizontal()
{
    return GetInput().mouseWheel.x;
}

static s32 GetMouseScrollVertical()
{
    return GetInput().mouseWheel.y;
}

static nkVec2 GetWindowMousePos()
{
    return GetInput().mousePos;
}

static nkVec2 GetRelativeMousePos()
{
    return GetInput().relativeMousePos;
}

static nkVec2 GetScreenMousePos()
{
    nkVec2 screenMouse = GetWindowMousePos();
    Rect screenBounds = GetScreenBounds();

    f32 windowWidth = NK_CAST(f32, GetWindowWidth());
    f32 windowHeight = NK_CAST(f32, GetWindowHeight());
    f32 screenWidth = GetScreenWidth();
    f32 screenHeight = GetScreenHeight();

    // How we calculate the screen mouse depends on the scale mode in-use.
    switch(GetScreenScaleMode())
    {
        case ScaleMode_None:
        {
            screenMouse.x -= screenBounds.x;
            screenMouse.y -= screenBounds.y;
        } break;
        case ScaleMode_Stretch:
        {
            screenMouse.x = nk_map_range(screenMouse.x, 0.0f,windowWidth, 0.0f,screenWidth);
            screenMouse.y = nk_map_range(screenMouse.y, 0.0f,windowHeight, 0.0f,screenHeight);
        } break;
        case ScaleMode_Letterbox:
        {
            f32 sx = windowWidth / screenWidth;
            f32 sy = windowHeight / screenHeight;
            f32 s = nk_min(sx,sy);
            screenMouse.x = (screenMouse.x - screenBounds.x) / s;
            screenMouse.y = (screenMouse.y - screenBounds.y) / s;
        } break;
        case ScaleMode_Pixel:
        {
            f32 sx = windowWidth / screenWidth;
            f32 sy = windowHeight / screenHeight;
            f32 s = nk_min(sx,sy);
            if(s < 1.0f) s = 1.0f; // Avoid scale of zero.
            screenMouse.x = (screenMouse.x - screenBounds.x) / floorf(s);
            screenMouse.y = (screenMouse.y - screenBounds.y) / floorf(s);
        } break;
        default:
        {
            // Nothing...
        } break;
    }

    return screenMouse;
}

static bool IsMouseButtonDown(MouseButton button)
{
    if(button == MouseButton_Invalid) return false;
    return (GetInput().currentMouseButtonState[button] != 0);
}

static bool IsMouseButtonUp(MouseButton button)
{
    if(button == MouseButton_Invalid) return false;
    return (GetInput().currentMouseButtonState[button] == 0);
}

static bool IsMouseButtonPressed(MouseButton button)
{
    if(button == MouseButton_Invalid) return false;
    return (GetInput().currentMouseButtonState[button] != 0 &&
            GetInput().previousMouseButtonState[button] == 0);
}

static bool IsMouseButtonReleased(MouseButton button)
{
    if(button == MouseButton_Invalid) return false;
    return (GetInput().currentMouseButtonState[button] == 0 &&
            GetInput().previousMouseButtonState[button] != 0);
}

//
// Gamepad
//

static bool HasGamepad()
{
    return GetInput().hasGamepad;
}

static bool IsButtonDown(GamepadButton button)
{
    if(!GetInput().hasGamepad) return false;
    if(button == GamepadButton_Invalid) return false;
    return (GetInput().currentButtonState[button] != 0);
}

static bool IsButtonUp(GamepadButton button)
{
    if(!GetInput().hasGamepad) return false;
    if(button == GamepadButton_Invalid) return false;
    return (GetInput().currentButtonState[button] == 0);
}

static bool IsButtonPressed(GamepadButton button)
{
    if(!GetInput().hasGamepad) return false;
    if(button == GamepadButton_Invalid) return false;
    return (GetInput().currentButtonState[button] != 0 &&
            GetInput().previousButtonState[button] == 0);
}

static bool IsButtonReleased(GamepadButton button)
{
    if(!GetInput().hasGamepad) return false;
    if(button == GamepadButton_Invalid) return false;
    return (GetInput().currentButtonState[button] == 0 &&
            GetInput().previousButtonState[button] != 0);
}

static s16 GetAxis(GamepadAxis axis)
{
    if(!GetInput().hasGamepad) return 0;
    if(axis == GamepadAxis_Invalid) return 0;
    return GetInput().currentAxisState[axis];
}

static bool IsRightStickUp()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_RightY] < -k_gamepadStickDeadzone);
}

static bool IsRightStickRight()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_RightX] > k_gamepadStickDeadzone);
}

static bool IsRightStickDown()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_RightY] > k_gamepadStickDeadzone);
}

static bool IsRightStickLeft()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_RightX] < -k_gamepadStickDeadzone);
}

static bool IsRightStickUpPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_RightY] >= -k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_RightY] < -k_gamepadStickDeadzone);
}

static bool IsRightStickRightPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_RightX] <= k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_RightX] > k_gamepadStickDeadzone);
}

static bool IsRightStickDownPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_RightY] <= k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_RightY] > k_gamepadStickDeadzone);
}

static bool IsRightStickLeftPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_RightX] >= -k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_RightX] < -k_gamepadStickDeadzone);
}

static bool IsLeftStickUp()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_LeftY] < -k_gamepadStickDeadzone);
}

static bool IsLeftStickRight()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_LeftX] > k_gamepadStickDeadzone);
}

static bool IsLeftStickDown()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_LeftY] > k_gamepadStickDeadzone);
}

static bool IsLeftStickLeft()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().currentAxisState[GamepadAxis_LeftX] < -k_gamepadStickDeadzone);
}

static bool IsLeftStickUpPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_LeftY] >= -k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_LeftY] < -k_gamepadStickDeadzone);
}

static bool IsLeftStickRightPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_LeftX] <= k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_LeftX] > k_gamepadStickDeadzone);
}

static bool IsLeftStickDownPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_LeftY] <= k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_LeftY] > k_gamepadStickDeadzone);
}

static bool IsLeftStickLeftPressed()
{
    if(!GetInput().hasGamepad) return false;
    return (GetInput().previousAxisState[GamepadAxis_LeftX] >= -k_gamepadStickDeadzone &&
            GetInput().currentAxisState[GamepadAxis_LeftX] < -k_gamepadStickDeadzone);
}
