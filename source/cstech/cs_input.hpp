#pragma once

CS_PUBLIC_SCOPE
{
    // Keyboard
    enum KeyCode
    {
        KeyCode_Invalid,
        KeyCode_F1,
        KeyCode_F2,
        KeyCode_F3,
        KeyCode_F4,
        KeyCode_F5,
        KeyCode_F6,
        KeyCode_F7,
        KeyCode_F8,
        KeyCode_F9,
        KeyCode_F10,
        KeyCode_F11,
        KeyCode_F12,
        KeyCode_0,
        KeyCode_1,
        KeyCode_2,
        KeyCode_3,
        KeyCode_4,
        KeyCode_5,
        KeyCode_6,
        KeyCode_7,
        KeyCode_8,
        KeyCode_9,
        KeyCode_Q,
        KeyCode_W,
        KeyCode_E,
        KeyCode_R,
        KeyCode_T,
        KeyCode_Y,
        KeyCode_U,
        KeyCode_I,
        KeyCode_O,
        KeyCode_P,
        KeyCode_A,
        KeyCode_S,
        KeyCode_D,
        KeyCode_F,
        KeyCode_G,
        KeyCode_H,
        KeyCode_J,
        KeyCode_K,
        KeyCode_L,
        KeyCode_Z,
        KeyCode_X,
        KeyCode_C,
        KeyCode_V,
        KeyCode_B,
        KeyCode_N,
        KeyCode_M,
        KeyCode_Escape,
        KeyCode_Tab,
        KeyCode_CapsLock,
        KeyCode_Backspace,
        KeyCode_Enter,
        KeyCode_Space,
        KeyCode_Up,
        KeyCode_Right,
        KeyCode_Down,
        KeyCode_Left,
        KeyCode_LeftShift,
        KeyCode_RightShift,
        KeyCode_LeftCtrl,
        KeyCode_RightCtrl,
        KeyCode_LeftAlt,
        KeyCode_RightAlt,
        KeyCode_TOTAL
    };

    CS_API bool IsKeyDown(KeyCode code);
    CS_API bool IsKeyUp(KeyCode code);
    CS_API bool IsKeyPressed(KeyCode code);
    CS_API bool IsKeyReleased(KeyCode code);

    CS_API bool IsAnyKeyDown();
    CS_API bool IsAnyKeyUp();
    CS_API bool IsAnyKeyPressed();
    CS_API bool IsAnyKeyReleased();

    // Mouse
    enum MouseButton
    {
        MouseButton_Invalid,
        MouseButton_Left,
        MouseButton_Middle,
        MouseButton_Right,
        MouseButton_TOTAL
    };

    CS_API s32 GetMouseScrollHorizontal();
    CS_API s32 GetMouseScrollVertical();

    CS_API Vec2 GetWindowMousePos();
    CS_API Vec2 GetRelativeMousePos();
    CS_API Vec2 GetScreenMousePos();

    CS_API bool IsMouseButtonDown(MouseButton button);
    CS_API bool IsMouseButtonUp(MouseButton button);
    CS_API bool IsMouseButtonPressed(MouseButton button);
    CS_API bool IsMouseButtonReleased(MouseButton button);

    // Gamepad
    enum GamepadButton
    {
        GamepadButton_Invalid,
        GamepadButton_A,
        GamepadButton_B,
        GamepadButton_X,
        GamepadButton_Y,
        GamepadButton_Select,
        GamepadButton_Start,
        GamepadButton_LeftStick,
        GamepadButton_RightStick,
        GamepadButton_LeftShoulder,
        GamepadButton_RightShoulder,
        GamepadButton_Up,
        GamepadButton_Right,
        GamepadButton_Down,
        GamepadButton_Left,
        GamepadButton_TOTAL
    };

    enum GamepadAxis
    {
        GamepadAxis_Invalid,
        GamepadAxis_LeftX,
        GamepadAxis_LeftY,
        GamepadAxis_RightX,
        GamepadAxis_RightY,
        GamepadAxis_LeftTrigger,
        GamepadAxis_RightTrigger,
        GamepadAxis_TOTAL
    };

    CS_API bool HasGamepad();

    CS_API bool IsButtonDown(GamepadButton button);
    CS_API bool IsButtonUp(GamepadButton button);
    CS_API bool IsButtonPressed(GamepadButton button);
    CS_API bool IsButtonReleased(GamepadButton button);

    CS_API s16 GetAxis(GamepadAxis axis);

    CS_API bool IsRightStickUp();
    CS_API bool IsRightStickRight();
    CS_API bool IsRightStickDown();
    CS_API bool IsRightStickLeft();

    CS_API bool IsRightStickUpPressed();
    CS_API bool IsRightStickRightPressed();
    CS_API bool IsRightStickDownPressed();
    CS_API bool IsRightStickLeftPressed();

    CS_API bool IsLeftStickUp();
    CS_API bool IsLeftStickRight();
    CS_API bool IsLeftStickDown();
    CS_API bool IsLeftStickLeft();

    CS_API bool IsLeftStickUpPressed();
    CS_API bool IsLeftStickRightPressed();
    CS_API bool IsLeftStickDownPressed();
    CS_API bool IsLeftStickLeftPressed();
}
