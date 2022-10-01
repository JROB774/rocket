#include "cs_input.hpp"
#include "cs_platform.hpp"
#include "cs_graphics.hpp"

using namespace cs;
using namespace gfx;

CS_PRIVATE_SCOPE
{
    static constexpr s16 k_gamepadStickDeadzone = 12000;
}

CS_PUBLIC_SCOPE
{
    //
    // Keyboard
    //

    CS_API bool IsKeyDown(KeyCode code)
    {
        if(code == KeyCode_Invalid) return false;
        return (GetInput().currentKeyState[code] != 0);
    }

    CS_API bool IsKeyUp(KeyCode code)
    {
        if(code == KeyCode_Invalid) return false;
        return (GetInput().currentKeyState[code] == 0);
    }

    CS_API bool IsKeyPressed(KeyCode code)
    {
        if(code == KeyCode_Invalid) return false;
        return (GetInput().currentKeyState[code] != 0 &&
                GetInput().previousKeyState[code] == 0);
    }

    CS_API bool IsKeyReleased(KeyCode code)
    {
        if(code == KeyCode_Invalid) return false;
        return (GetInput().currentKeyState[code] == 0 &&
                GetInput().previousKeyState[code] != 0);
    }

    CS_API bool IsAnyKeyDown()
    {
        for(s32 i=0; i<CS_CAST(s32, KeyCode_TOTAL); ++i)
            if(IsKeyDown(CS_CAST(KeyCode, i))) return true;
        return false;
    }

    CS_API bool IsAnyKeyUp()
    {
        for(s32 i=0; i<CS_CAST(s32, KeyCode_TOTAL); ++i)
            if(IsKeyUp(CS_CAST(KeyCode, i))) return true;
        return false;
    }

    CS_API bool IsAnyKeyPressed()
    {
        for(s32 i=0; i<CS_CAST(s32, KeyCode_TOTAL); ++i)
            if(IsKeyPressed(CS_CAST(KeyCode, i))) return true;
        return false;
    }

    CS_API bool IsAnyKeyReleased()
    {
        for(s32 i=0; i<CS_CAST(s32, KeyCode_TOTAL); ++i)
            if(IsKeyReleased(CS_CAST(KeyCode, i))) return true;
        return false;
    }

    //
    // Mouse
    //

    CS_API s32 GetMouseScrollHorizontal()
    {
        return GetInput().mouseWheel.x;
    }

    CS_API s32 GetMouseScrollVertical()
    {
        return GetInput().mouseWheel.y;
    }

    CS_API Vec2 GetWindowMousePos()
    {
        return GetInput().mousePos;
    }

    CS_API Vec2 GetRelativeMousePos()
    {
        return GetInput().relativeMousePos;
    }

    CS_API Vec2 GetScreenMousePos()
    {
        Vec2 screenMouse = GetWindowMousePos();
        Rect screenBounds = GetScreenBounds();

        f32 windowWidth = CS_CAST(f32, GetWindowWidth());
        f32 windowHeight = CS_CAST(f32, GetWindowHeight());
        f32 screenWidth = GetScreenWidth();
        f32 screenHeight = GetScreenHeight();

        // How we calculate the screen mouse depends on the scale mode in-use.
        switch(GetScreenScaleMode())
        {
            case(ScaleMode_None):
            {
                screenMouse.x -= screenBounds.x;
                screenMouse.y -= screenBounds.y;
            } break;
            case(ScaleMode_Stretch):
            {
                screenMouse.x = csm::MapRange(screenMouse.x, 0.0f,windowWidth, 0.0f,screenWidth);
                screenMouse.y = csm::MapRange(screenMouse.y, 0.0f,windowHeight, 0.0f,screenHeight);
            } break;
            case(ScaleMode_Letterbox):
            {
                f32 sx = windowWidth / screenWidth;
                f32 sy = windowHeight / screenHeight;
                f32 s = csm::Min(sx,sy);
                screenMouse.x = (screenMouse.x - screenBounds.x) / s;
                screenMouse.y = (screenMouse.y - screenBounds.y) / s;
            } break;
            case(ScaleMode_Pixel):
            {
                f32 sx = windowWidth / screenWidth;
                f32 sy = windowHeight / screenHeight;
                f32 s = csm::Min(sx,sy);
                if(s < 1.0f) s = 1.0f; // Avoid scale of zero.
                screenMouse.x = (screenMouse.x - screenBounds.x) / floorf(s);
                screenMouse.y = (screenMouse.y - screenBounds.y) / floorf(s);
            } break;
        }

        return screenMouse;
    }

    CS_API bool IsMouseButtonDown(MouseButton button)
    {
        if(button == MouseButton_Invalid) return false;
        return (GetInput().currentMouseButtonState[button] != 0);
    }

    CS_API bool IsMouseButtonUp(MouseButton button)
    {
        if(button == MouseButton_Invalid) return false;
        return (GetInput().currentMouseButtonState[button] == 0);
    }

    CS_API bool IsMouseButtonPressed(MouseButton button)
    {
        if(button == MouseButton_Invalid) return false;
        return (GetInput().currentMouseButtonState[button] != 0 &&
                GetInput().previousMouseButtonState[button] == 0);
    }

    CS_API bool IsMouseButtonReleased(MouseButton button)
    {
        if(button == MouseButton_Invalid) return false;
        return (GetInput().currentMouseButtonState[button] == 0 &&
                GetInput().previousMouseButtonState[button] != 0);
    }

    //
    // Gamepad
    //

    CS_API bool HasGamepad()
    {
        return GetInput().hasGamepad;
    }

    CS_API bool IsButtonDown(GamepadButton button)
    {
        if(!GetInput().hasGamepad) return false;
        if(button == GamepadButton_Invalid) return false;
        return (GetInput().currentButtonState[button] != 0);
    }

    CS_API bool IsButtonUp(GamepadButton button)
    {
        if(!GetInput().hasGamepad) return false;
        if(button == GamepadButton_Invalid) return false;
        return (GetInput().currentButtonState[button] == 0);
    }

    CS_API bool IsButtonPressed(GamepadButton button)
    {
        if(!GetInput().hasGamepad) return false;
        if(button == GamepadButton_Invalid) return false;
        return (GetInput().currentButtonState[button] != 0 &&
                GetInput().previousButtonState[button] == 0);
    }

    CS_API bool IsButtonReleased(GamepadButton button)
    {
        if(!GetInput().hasGamepad) return false;
        if(button == GamepadButton_Invalid) return false;
        return (GetInput().currentButtonState[button] == 0 &&
                GetInput().previousButtonState[button] != 0);
    }

    CS_API s16 GetAxis(GamepadAxis axis)
    {
        if(!GetInput().hasGamepad) return 0;
        if(axis == GamepadAxis_Invalid) return 0;
        return GetInput().currentAxisState[axis];
    }

    CS_API bool IsRightStickUp()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_RightY] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickRight()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_RightX] > k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickDown()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_RightY] > k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickLeft()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_RightX] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickUpPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_RightY] >= -k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_RightY] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickRightPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_RightX] <= k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_RightX] > k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickDownPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_RightY] <= k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_RightY] > k_gamepadStickDeadzone);
    }

    CS_API bool IsRightStickLeftPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_RightX] >= -k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_RightX] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickUp()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_LeftY] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickRight()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_LeftX] > k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickDown()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_LeftY] > k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickLeft()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().currentAxisState[GamepadAxis_LeftX] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickUpPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_LeftY] >= -k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_LeftY] < -k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickRightPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_LeftX] <= k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_LeftX] > k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickDownPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_LeftY] <= k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_LeftY] > k_gamepadStickDeadzone);
    }

    CS_API bool IsLeftStickLeftPressed()
    {
        if(!GetInput().hasGamepad) return false;
        return (GetInput().previousAxisState[GamepadAxis_LeftX] >= -k_gamepadStickDeadzone &&
                GetInput().currentAxisState[GamepadAxis_LeftX] < -k_gamepadStickDeadzone);
    }
}
