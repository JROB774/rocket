struct PlatformContext
{
    SDL_Window* window;
    SDL_GLContext glContext;
    SDL_GameController* gamepad;
    s32 cachedWindowXPos;
    s32 cachedWindowYPos;
    s32 cachedWindowWidth;
    s32 cachedWindowHeight;
    bool maximized;
    bool fullscreen;
    std::string execPath;
    InputState input;
};

static PlatformContext s_context;
static AppConfig s_appConfig;

static void RemoveGamepad()
{
    if(s_context.gamepad)
    {
        SDL_GameControllerClose(s_context.gamepad);
        s_context.gamepad = NULL;
    }
}

static void AddGamepad()
{
    // Search for a plugged in gamepad and if there is one use it.
    RemoveGamepad();
    for(s32 i=0; i<SDL_NumJoysticks(); ++i)
    {
        if(SDL_IsGameController(i))
        {
            s_context.gamepad = SDL_GameControllerOpen(i);
            if(s_context.gamepad)
                break;
        }
    }
}

static SDL_Scancode MapKeyCodeToSDLScancode(KeyCode code)
{
    static const std::map<KeyCode,SDL_Scancode> k_keyCodes
    {
        { KeyCode_Invalid, SDL_SCANCODE_UNKNOWN },
        { KeyCode_F1, SDL_SCANCODE_F1 },
        { KeyCode_F2, SDL_SCANCODE_F2 },
        { KeyCode_F3, SDL_SCANCODE_F3 },
        { KeyCode_F4, SDL_SCANCODE_F4 },
        { KeyCode_F5, SDL_SCANCODE_F5 },
        { KeyCode_F6, SDL_SCANCODE_F6 },
        { KeyCode_F7, SDL_SCANCODE_F7 },
        { KeyCode_F8, SDL_SCANCODE_F8 },
        { KeyCode_F9, SDL_SCANCODE_F9 },
        { KeyCode_F10, SDL_SCANCODE_F10 },
        { KeyCode_F11, SDL_SCANCODE_F11 },
        { KeyCode_F12, SDL_SCANCODE_F12 },
        { KeyCode_0, SDL_SCANCODE_0 },
        { KeyCode_1, SDL_SCANCODE_1 },
        { KeyCode_2, SDL_SCANCODE_2 },
        { KeyCode_3, SDL_SCANCODE_3 },
        { KeyCode_4, SDL_SCANCODE_4 },
        { KeyCode_5, SDL_SCANCODE_5 },
        { KeyCode_6, SDL_SCANCODE_6 },
        { KeyCode_7, SDL_SCANCODE_7 },
        { KeyCode_8, SDL_SCANCODE_8 },
        { KeyCode_9, SDL_SCANCODE_9 },
        { KeyCode_Q, SDL_SCANCODE_Q },
        { KeyCode_W, SDL_SCANCODE_W },
        { KeyCode_E, SDL_SCANCODE_E },
        { KeyCode_R, SDL_SCANCODE_R },
        { KeyCode_T, SDL_SCANCODE_T },
        { KeyCode_Y, SDL_SCANCODE_Y },
        { KeyCode_U, SDL_SCANCODE_U },
        { KeyCode_I, SDL_SCANCODE_I },
        { KeyCode_O, SDL_SCANCODE_O },
        { KeyCode_P, SDL_SCANCODE_P },
        { KeyCode_A, SDL_SCANCODE_A },
        { KeyCode_S, SDL_SCANCODE_S },
        { KeyCode_D, SDL_SCANCODE_D },
        { KeyCode_F, SDL_SCANCODE_F },
        { KeyCode_G, SDL_SCANCODE_G },
        { KeyCode_H, SDL_SCANCODE_H },
        { KeyCode_J, SDL_SCANCODE_J },
        { KeyCode_K, SDL_SCANCODE_K },
        { KeyCode_L, SDL_SCANCODE_L },
        { KeyCode_Z, SDL_SCANCODE_Z },
        { KeyCode_X, SDL_SCANCODE_X },
        { KeyCode_C, SDL_SCANCODE_C },
        { KeyCode_V, SDL_SCANCODE_V },
        { KeyCode_B, SDL_SCANCODE_B },
        { KeyCode_N, SDL_SCANCODE_N },
        { KeyCode_M, SDL_SCANCODE_M },
        { KeyCode_Escape, SDL_SCANCODE_ESCAPE },
        { KeyCode_Tab, SDL_SCANCODE_TAB },
        { KeyCode_CapsLock, SDL_SCANCODE_CAPSLOCK },
        { KeyCode_Backspace, SDL_SCANCODE_BACKSPACE },
        { KeyCode_Enter, SDL_SCANCODE_RETURN },
        { KeyCode_Space, SDL_SCANCODE_SPACE },
        { KeyCode_Up, SDL_SCANCODE_UP },
        { KeyCode_Right, SDL_SCANCODE_RIGHT },
        { KeyCode_Down, SDL_SCANCODE_DOWN },
        { KeyCode_Left, SDL_SCANCODE_LEFT },
        { KeyCode_LeftShift, SDL_SCANCODE_LSHIFT },
        { KeyCode_RightShift, SDL_SCANCODE_RSHIFT },
        { KeyCode_LeftCtrl, SDL_SCANCODE_LCTRL },
        { KeyCode_RightCtrl, SDL_SCANCODE_RCTRL },
        { KeyCode_LeftAlt, SDL_SCANCODE_LALT },
        { KeyCode_RightAlt, SDL_SCANCODE_RALT }
    };

    return (k_keyCodes.find(code) != k_keyCodes.end()) ? k_keyCodes.at(code) : SDL_SCANCODE_UNKNOWN;
}

static s32 MapMouseButtonToSDLButton(MouseButton button)
{
    static const std::map<MouseButton,s32> k_mouseButtons
    {
        { MouseButton_Invalid, 0 },
        { MouseButton_Left, SDL_BUTTON_LEFT },
        { MouseButton_Middle, SDL_BUTTON_MIDDLE },
        { MouseButton_Right, SDL_BUTTON_RIGHT }
    };

    return (k_mouseButtons.find(button) != k_mouseButtons.end()) ? k_mouseButtons.at(button) : 0;
}

static SDL_GameControllerButton MapGamepadButtonToSDLGameControllerButton(GamepadButton button)
{
    static const std::map<GamepadButton,SDL_GameControllerButton> k_gamepadButtons
    {
        { GamepadButton_Invalid, SDL_CONTROLLER_BUTTON_INVALID },
        { GamepadButton_A, SDL_CONTROLLER_BUTTON_A },
        { GamepadButton_B, SDL_CONTROLLER_BUTTON_B },
        { GamepadButton_X, SDL_CONTROLLER_BUTTON_X },
        { GamepadButton_Y, SDL_CONTROLLER_BUTTON_Y },
        { GamepadButton_Select, SDL_CONTROLLER_BUTTON_BACK },
        { GamepadButton_Start, SDL_CONTROLLER_BUTTON_START },
        { GamepadButton_LeftStick, SDL_CONTROLLER_BUTTON_LEFTSTICK },
        { GamepadButton_RightStick, SDL_CONTROLLER_BUTTON_RIGHTSTICK },
        { GamepadButton_LeftShoulder, SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
        { GamepadButton_RightShoulder, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
        { GamepadButton_Up, SDL_CONTROLLER_BUTTON_DPAD_UP },
        { GamepadButton_Right, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
        { GamepadButton_Down, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
        { GamepadButton_Left, SDL_CONTROLLER_BUTTON_DPAD_LEFT }
    };

    return (k_gamepadButtons.find(button) != k_gamepadButtons.end()) ? k_gamepadButtons.at(button) : SDL_CONTROLLER_BUTTON_INVALID;
}

static SDL_GameControllerAxis MapGamepadAxisToSDLGameControllerAxis(GamepadAxis axis)
{
    static const std::map<GamepadAxis,SDL_GameControllerAxis> k_gamepadAxes
    {
        { GamepadAxis_Invalid, SDL_CONTROLLER_AXIS_INVALID },
        { GamepadAxis_LeftX, SDL_CONTROLLER_AXIS_LEFTX },
        { GamepadAxis_LeftY, SDL_CONTROLLER_AXIS_LEFTY },
        { GamepadAxis_RightX, SDL_CONTROLLER_AXIS_RIGHTX },
        { GamepadAxis_RightY, SDL_CONTROLLER_AXIS_RIGHTY },
        { GamepadAxis_LeftTrigger, SDL_CONTROLLER_AXIS_TRIGGERLEFT },
        { GamepadAxis_RightTrigger, SDL_CONTROLLER_AXIS_TRIGGERRIGHT }
    };

    return (k_gamepadAxes.find(axis) != k_gamepadAxes.end()) ? k_gamepadAxes.at(axis) : SDL_CONTROLLER_AXIS_INVALID;
}

static void UpdateInputState()
{
    // Update the has gamepad state.
    s_context.input.hasGamepad = (s_context.gamepad != NULL);

    // Update the mouse state.
    s32 mouseX,mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    s_context.input.mousePos = { NK_CAST(f32, mouseX), NK_CAST(f32, mouseY) };
    SDL_GetRelativeMouseState(&mouseX,&mouseY);
    s_context.input.relativeMousePos = { NK_CAST(f32, mouseX), NK_CAST(f32, mouseY) };
    s_context.input.mouseWheel = { 0,0 };

    // Update the keyboard state.
    memcpy(s_context.input.previousKeyState, s_context.input.currentKeyState, sizeof(s_context.input.previousKeyState));
    const u8* sdlKeyboardState = SDL_GetKeyboardState(NULL);
    for(s32 i=0; i<KeyCode_TOTAL; ++i)
        s_context.input.currentKeyState[i] = (sdlKeyboardState[MapKeyCodeToSDLScancode(NK_CAST(KeyCode, i))] != 0);

    // Update the mouse button state.
    u32 sdlMouse = SDL_GetMouseState(NULL,NULL);
    memcpy(s_context.input.previousMouseButtonState, s_context.input.currentMouseButtonState, sizeof(s_context.input.previousMouseButtonState));
    for(s32 i=0; i<MouseButton_TOTAL; ++i)
        s_context.input.currentMouseButtonState[i] = ((sdlMouse & SDL_BUTTON(MapMouseButtonToSDLButton(NK_CAST(MouseButton, i)))) != 0);

    // Update the gamepad.
    if(s_context.gamepad)
    {
        // Button state.
        memcpy(s_context.input.previousButtonState, s_context.input.currentButtonState, sizeof(s_context.input.previousButtonState));
        for(s32 i=0; i<GamepadButton_TOTAL; ++i)
        {
            u8 sdlButton = SDL_GameControllerGetButton(s_context.gamepad, MapGamepadButtonToSDLGameControllerButton(NK_CAST(GamepadButton, i)));
            s_context.input.currentButtonState[i] = (sdlButton != 0);
        }
        // Axis state.
        for(s32 i=0; i<GamepadAxis_TOTAL; ++i)
        {
            s16 sdlAxis = SDL_GameControllerGetAxis(s_context.gamepad, MapGamepadAxisToSDLGameControllerAxis(NK_CAST(GamepadAxis, i)));
            s_context.input.currentButtonState[i] = sdlAxis;
        }
    }
    else
    {
        memset(s_context.input.previousButtonState, 0, sizeof(s_context.input.previousButtonState));
        memset(s_context.input.currentButtonState, 0, sizeof(s_context.input.currentButtonState));
        memset(s_context.input.previousAxisState, 0, sizeof(s_context.input.previousAxisState));
        memset(s_context.input.currentAxisState, 0, sizeof(s_context.input.currentAxisState));
    }
}

static f32 CounterToSeconds(u64 counter, u64 frequency)
{
    return (NK_CAST(f32, counter) / NK_CAST(f32, frequency));
}

static void CacheWindowBounds()
{
    if(IsMaximized()) SDL_RestoreWindow(s_context.window);
    SDL_GetWindowPosition(s_context.window, &s_context.cachedWindowXPos, &s_context.cachedWindowYPos);
    SDL_GetWindowSize(s_context.window, &s_context.cachedWindowWidth, &s_context.cachedWindowHeight);
    if(IsMaximized()) SDL_MaximizeWindow(s_context.window);
}

//
// Misc
//

static const AppConfig& GetAppConfig()
{
    return s_appConfig;
}

static std::string GetExecPath()
{
    return s_context.execPath;
}

static void FatalError(const char* format, ...)
{
    char message[1024] = {};
    va_list args;
    va_start(args, format);
    vsnprintf(message, ARRAY_SIZE(message), format, args);
    va_end(args);
    fprintf(stderr, "%s\n", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, s_context.window);
    abort();
}

//
// Window
//

static void PositionWindow(s32 x, s32 y)
{
    SDL_SetWindowPosition(s_context.window, x, y);
}

static void ResizeWindow(s32 w, s32 h)
{
    SDL_SetWindowSize(s_context.window, w, h);
}

static void MaximizeWindow(bool enable)
{
    if(s_context.maximized != enable)
    {
        // We don't set the context's maximized state here, we instead do it in the event loop so that it also gets set when the maximize button is toggled.
        if(enable) SDL_MaximizeWindow(s_context.window);
        else SDL_RestoreWindow(s_context.window);
    }
}

static bool IsMaximized()
{
    return s_context.maximized;
}

static void FullscreenWindow(bool enable)
{
    if(s_context.fullscreen != enable)
    {
        if(enable) CacheWindowBounds();
        u32 flags = (enable) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        SDL_SetWindowFullscreen(s_context.window, flags);
        s_context.fullscreen = enable;
    }
}

static bool IsFullscreen()
{
    return s_context.fullscreen;
}

static s32 GetWindowX()
{
    s32 x;
    SDL_GetWindowPosition(s_context.window, &x, NULL);
    return x;
}

static s32 GetWindowY()
{
    s32 y;
    SDL_GetWindowPosition(s_context.window, NULL, &y);
    return y;
}

static s32 GetWindowWidth()
{
    s32 width;
    SDL_GetWindowSize(s_context.window, &width, NULL);
    return width;
}

static s32 GetWindowHeight()
{
    s32 height;
    SDL_GetWindowSize(s_context.window, NULL, &height);
    return height;
}

static void ShowCursor(bool show)
{
    SDL_ShowCursor(((show) ? SDL_ENABLE : SDL_DISABLE));
}

static bool IsCursorVisible()
{
    return (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE);
}

static void LockMouse(bool lock)
{
    SDL_SetRelativeMouseMode((lock) ? SDL_TRUE : SDL_FALSE);
}

static bool IsMouseLocked()
{
    return SDL_GetRelativeMouseMode();
}

//
// File System
//

static bool DoesFileExist(std::string fileName)
{
    return (std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName));
}

static bool DoesPathExist(std::string pathName)
{
    return (std::filesystem::exists(pathName) && std::filesystem::is_directory(pathName));
}

static bool IsFile(std::string fileName)
{
    return std::filesystem::is_regular_file(fileName);
}

static bool IsPath(std::string pathName)
{
    return std::filesystem::is_directory(pathName);
}

static std::string ValidatePath(std::string pathName)
{
    std::replace(pathName.begin(), pathName.end(), '\\', '/');
    if(IsPath(pathName) && pathName.back() != '/') pathName += "/";
    return pathName;
}

static bool CreatePath(std::string pathName)
{
    std::filesystem::create_directories(pathName);
    return DoesPathExist(pathName);
}

static size_t GetSizeOfFile(std::string fileName)
{
    return std::filesystem::file_size(fileName);
}

static std::string StripFileExtension(std::string fileName)
{
    return std::filesystem::path(fileName).replace_extension("").string();
}

static std::string StripFilePath(std::string fileName)
{
    return std::filesystem::path(fileName).filename().string();
}

static std::string StripFileExtensionAndPath(std::string fileName)
{
    return std::filesystem::path(fileName).replace_extension("").filename().string();
}

static std::string StripFileName(std::string fileName)
{
    return std::filesystem::path(fileName).remove_filename().string();
}

static std::string GetFileExtension(std::string fileName)
{
    return std::filesystem::path(fileName).extension().string();
}

static std::string GetFilePath(std::string fileName)
{
    return std::filesystem::path(fileName).remove_filename().string();
}

static std::string ReadEntireFile(std::string fileName)
{
    std::ifstream file(fileName);
    if(!file.is_open()) return "";
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

static void WriteEntireFile(std::string fileName, std::string content)
{
    std::ofstream file(fileName);
    if(!file.is_open()) return;
    file << content;
}

static std::vector<u8> ReadBinaryFile(std::string fileName)
{
    std::vector<u8> data;
    std::ifstream file(fileName, std::ios::binary);
    if(!file.is_open()) return data;
    data.resize(GetSizeOfFile(fileName));
    file.read(NK_CAST(char*, &data[0]), data.size()*sizeof(u8));
    return data;
}

static void WriteBinaryFile(std::string fileName, void* data, size_t size)
{
    std::ofstream file(fileName, std::ios::binary);
    if(!file.is_open()) return;
    file.write(NK_CAST(const char*, data), size);
}

static void ListPathFiles(std::string pathName, std::vector<std::string>& files, bool recursive)
{
    if(!DoesPathExist(pathName)) return;
    if(recursive)
    {
        for(auto& p: std::filesystem::recursive_directory_iterator(pathName))
            if(IsFile(p.path().string())) files.push_back(p.path().string());
    }
    else
    {
        for(auto& p: std::filesystem::directory_iterator(pathName))
            if(IsFile(p.path().string())) files.push_back(p.path().string());
    }
}

//
// Input
//

static const InputState& GetInput()
{
    return s_context.input;
}

//
// Main
//

#ifndef __EMSCRIPTEN__
#include "main_native.cpp"
#else
#include "main_web.cpp"
#endif
