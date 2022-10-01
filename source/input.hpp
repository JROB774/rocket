// Keyboard
enum KeyCode
{
    KeyCode_Invalid,
    KeyCode_F1, KeyCode_F2, KeyCode_F3, KeyCode_F4, KeyCode_F5, KeyCode_F6,
    KeyCode_F7, KeyCode_F8, KeyCode_F9, KeyCode_F10, KeyCode_F11, KeyCode_F12,
    KeyCode_0, KeyCode_1, KeyCode_2, KeyCode_3, KeyCode_4,
    KeyCode_5, KeyCode_6, KeyCode_7, KeyCode_8, KeyCode_9,
    KeyCode_Q, KeyCode_W, KeyCode_E, KeyCode_R, KeyCode_T, KeyCode_Y, KeyCode_U,
    KeyCode_I, KeyCode_O, KeyCode_P, KeyCode_A, KeyCode_S, KeyCode_D, KeyCode_F,
    KeyCode_G, KeyCode_H, KeyCode_J, KeyCode_K, KeyCode_L, KeyCode_Z, KeyCode_X,
    KeyCode_C, KeyCode_V, KeyCode_B, KeyCode_N, KeyCode_M,
    KeyCode_Escape,
    KeyCode_Tab,
    KeyCode_CapsLock,
    KeyCode_Backspace,
    KeyCode_Enter,
    KeyCode_Space,
    KeyCode_Up, KeyCode_Right, KeyCode_Down, KeyCode_Left,
    KeyCode_LeftShift, KeyCode_RightShift,
    KeyCode_LeftCtrl, KeyCode_RightCtrl,
    KeyCode_LeftAlt, KeyCode_RightAlt,
    KeyCode_TOTAL
};

static bool IsKeyDown(KeyCode code);
static bool IsKeyUp(KeyCode code);
static bool IsKeyPressed(KeyCode code);
static bool IsKeyReleased(KeyCode code);

static bool IsAnyKeyDown();
static bool IsAnyKeyUp();
static bool IsAnyKeyPressed();
static bool IsAnyKeyReleased();

// Mouse
enum MouseButton
{
    MouseButton_Invalid,
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
    MouseButton_TOTAL
};

static s32 GetMouseScrollHorizontal();
static s32 GetMouseScrollVertical();

static Vec2 GetWindowMousePos();
static Vec2 GetRelativeMousePos();
static Vec2 GetScreenMousePos();

static bool IsMouseButtonDown(MouseButton button);
static bool IsMouseButtonUp(MouseButton button);
static bool IsMouseButtonPressed(MouseButton button);
static bool IsMouseButtonReleased(MouseButton button);

// Gamepad
enum GamepadButton
{
    GamepadButton_Invalid,
    GamepadButton_A, GamepadButton_B, GamepadButton_X, GamepadButton_Y,
    GamepadButton_Select,
    GamepadButton_Start,
    GamepadButton_LeftStick, GamepadButton_RightStick,
    GamepadButton_LeftShoulder, GamepadButton_RightShoulder,
    GamepadButton_Up, GamepadButton_Right, GamepadButton_Down, GamepadButton_Left,
    GamepadButton_TOTAL
};

enum GamepadAxis
{
    GamepadAxis_Invalid,
    GamepadAxis_LeftX, GamepadAxis_LeftY,
    GamepadAxis_RightX, GamepadAxis_RightY,
    GamepadAxis_LeftTrigger, GamepadAxis_RightTrigger,
    GamepadAxis_TOTAL
};

static bool HasGamepad();

static bool IsButtonDown(GamepadButton button);
static bool IsButtonUp(GamepadButton button);
static bool IsButtonPressed(GamepadButton button);
static bool IsButtonReleased(GamepadButton button);

static s16 GetAxis(GamepadAxis axis);

static bool IsRightStickUp();
static bool IsRightStickRight();
static bool IsRightStickDown();
static bool IsRightStickLeft();

static bool IsRightStickUpPressed();
static bool IsRightStickRightPressed();
static bool IsRightStickDownPressed();
static bool IsRightStickLeftPressed();

static bool IsLeftStickUp();
static bool IsLeftStickRight();
static bool IsLeftStickDown();
static bool IsLeftStickLeft();

static bool IsLeftStickUpPressed();
static bool IsLeftStickRightPressed();
static bool IsLeftStickDownPressed();
static bool IsLeftStickLeftPressed();
