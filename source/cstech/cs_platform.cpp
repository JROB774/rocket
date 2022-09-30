#include "cs_platform.hpp"
#include "cs_debug.hpp"
#include "cs_graphics.hpp"
#include "cs_assets.hpp"
#include "cs_audio.hpp"
#include "cs_memory.hpp"
#include "cs_utility.hpp"

#include <filesystem>
#include <fstream>

#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glew.c>

using namespace cs;
using namespace gfx;

CS_PRIVATE_SCOPE
{
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
        std::string dataPath;
        InputState input;
        f32 currentFPS;
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
        s_context.input.mousePos = Vec2(CS_CAST(f32, mouseX), CS_CAST(f32, mouseY));
        SDL_GetRelativeMouseState(&mouseX,&mouseY);
        s_context.input.relativeMousePos = Vec2(CS_CAST(f32, mouseX), CS_CAST(f32, mouseY));
        s_context.input.mouseWheel = Vec2i(0,0);

        // Update the keyboard state.
        memcpy(s_context.input.previousKeyState, s_context.input.currentKeyState, sizeof(s_context.input.previousKeyState));
        const u8* sdlKeyboardState = SDL_GetKeyboardState(NULL);
        for(s32 i=0; i<KeyCode_TOTAL; ++i)
            s_context.input.currentKeyState[i] = (sdlKeyboardState[MapKeyCodeToSDLScancode(CS_CAST(KeyCode, i))] != 0);

        // Update the mouse button state.
        u32 sdlMouse = SDL_GetMouseState(NULL,NULL);
        memcpy(s_context.input.previousMouseButtonState, s_context.input.currentMouseButtonState, sizeof(s_context.input.previousMouseButtonState));
        for(s32 i=0; i<MouseButton_TOTAL; ++i)
            s_context.input.currentMouseButtonState[i] = ((sdlMouse & SDL_BUTTON(MapMouseButtonToSDLButton(CS_CAST(MouseButton, i)))) != 0);

        // Update the gamepad.
        if(s_context.gamepad)
        {
            // Button state.
            memcpy(s_context.input.previousButtonState, s_context.input.currentButtonState, sizeof(s_context.input.previousButtonState));
            for(s32 i=0; i<GamepadButton_TOTAL; ++i)
            {
                u8 sdlButton = SDL_GameControllerGetButton(s_context.gamepad, MapGamepadButtonToSDLGameControllerButton(CS_CAST(GamepadButton, i)));
                s_context.input.currentButtonState[i] = (sdlButton != 0);
            }
            // Axis state.
            for(s32 i=0; i<GamepadAxis_TOTAL; ++i)
            {
                s16 sdlAxis = SDL_GameControllerGetAxis(s_context.gamepad, MapGamepadAxisToSDLGameControllerAxis(CS_CAST(GamepadAxis, i)));
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
        return (CS_CAST(f32, counter) / CS_CAST(f32, frequency));
    }

    static void CacheWindowBounds()
    {
        if(IsMaximized()) SDL_RestoreWindow(s_context.window);
        SDL_GetWindowPosition(s_context.window, &s_context.cachedWindowXPos, &s_context.cachedWindowYPos);
        SDL_GetWindowSize(s_context.window, &s_context.cachedWindowWidth, &s_context.cachedWindowHeight);
        if(IsMaximized()) SDL_MaximizeWindow(s_context.window);
    }
}

CS_PUBLIC_SCOPE
{
    CS_API const AppConfig& GetAppConfig()
    {
        return s_appConfig;
    }

    CS_API f32 GetCurrentFPS()
    {
        return s_context.currentFPS;
    }

    CS_API std::string GetExecPath()
    {
        return s_context.execPath;
    }

    CS_API std::string GetDataPath()
    {
        return s_context.dataPath;
    }

    CS_API void ResetWindow()
    {
        WindowConfig& config = s_appConfig.window;
        EnableVSync(config.vSync);
        MaximizeWindow(config.maximized);
        FullscreenWindow(config.fullscreen);
        ResizeWindow(config.size.x, config.size.y);
        PositionWindow(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    CS_API void PositionWindow(s32 x, s32 y)
    {
        SDL_SetWindowPosition(s_context.window, x, y);
    }

    CS_API void ResizeWindow(s32 w, s32 h)
    {
        SDL_SetWindowSize(s_context.window, w, h);
    }

    CS_API void MaximizeWindow(bool enable)
    {
        if(s_context.maximized != enable)
        {
            // We don't set the context's maximized state here, we instead do it in the event loop so that it also gets set when the maximize button is toggled.
            if(enable) SDL_MaximizeWindow(s_context.window);
            else SDL_RestoreWindow(s_context.window);
        }
    }

    CS_API bool IsMaximized()
    {
        return s_context.maximized;
    }

    CS_API void FullscreenWindow(bool enable)
    {
        if(s_context.fullscreen != enable)
        {
            if(enable) CacheWindowBounds();
            u32 flags = (enable) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
            SDL_SetWindowFullscreen(s_context.window, flags);
            s_context.fullscreen = enable;
        }
    }

    CS_API bool IsFullscreen()
    {
        return s_context.fullscreen;
    }

    CS_API void EnableVSync(bool enable)
    {
        if(SDL_GL_SetSwapInterval((enable) ? 1 : 0))
            CS_DEBUG_LOG("Failed to set the swap interval!");
    }

    CS_API bool IsVSyncOn()
    {
        return (SDL_GL_GetSwapInterval() == 1);
    }

    CS_API s32 GetWindowX()
    {
        s32 x;
        SDL_GetWindowPosition(s_context.window, &x, NULL);
        return x;
    }

    CS_API s32 GetWindowY()
    {
        s32 y;
        SDL_GetWindowPosition(s_context.window, NULL, &y);
        return y;
    }

    CS_API s32 GetWindowWidth()
    {
        s32 width;
        SDL_GetWindowSize(s_context.window, &width, NULL);
        return width;
    }

    CS_API s32 GetWindowHeight()
    {
        s32 height;
        SDL_GetWindowSize(s_context.window, NULL, &height);
        return height;
    }

    CS_API void ShowCursor(bool show)
    {
        SDL_ShowCursor(((show) ? SDL_ENABLE : SDL_DISABLE));
    }

    CS_API bool IsCursorVisible()
    {
        return (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE);
    }

    CS_API void LockMouse(bool lock)
    {
        SDL_SetRelativeMouseMode((lock) ? SDL_TRUE : SDL_FALSE);
    }

    CS_API bool IsMouseLocked()
    {
        return SDL_GetRelativeMouseMode();
    }

    //
    // File System
    //

    CS_API bool DoesFileExist(std::string fileName)
    {
        return (std::filesystem::exists(fileName) && std::filesystem::is_regular_file(fileName));
    }

    CS_API bool DoesPathExist(std::string pathName)
    {
        return (std::filesystem::exists(pathName) && std::filesystem::is_directory(pathName));
    }

    CS_API bool IsFile(std::string fileName)
    {
        return std::filesystem::is_regular_file(fileName);
    }

    CS_API bool IsPath(std::string pathName)
    {
        return std::filesystem::is_directory(pathName);
    }

    CS_API std::string ValidatePath(std::string pathName)
    {
        std::replace(pathName.begin(), pathName.end(), '\\', '/');
        if(IsPath(pathName) && pathName.back() != '/') pathName += "/";
        return pathName;
    }

    CS_API bool CreatePath(std::string pathName)
    {
        std::filesystem::create_directories(pathName);
        return DoesPathExist(pathName);
    }

    CS_API size_t GetSizeOfFile(std::string fileName)
    {
        return std::filesystem::file_size(fileName);
    }

    CS_API std::string StripFileExtension(std::string fileName)
    {
        return std::filesystem::path(fileName).replace_extension("").string();
    }

    CS_API std::string StripFilePath(std::string fileName)
    {
        return std::filesystem::path(fileName).filename().string();
    }

    CS_API std::string StripFileExtensionAndPath(std::string fileName)
    {
        return std::filesystem::path(fileName).replace_extension("").filename().string();
    }

    CS_API std::string StripFileName(std::string fileName)
    {
        return std::filesystem::path(fileName).remove_filename().string();
    }

    CS_API std::string GetFileExtension(std::string fileName)
    {
        return std::filesystem::path(fileName).extension().string();
    }

    CS_API std::string GetFilePath(std::string fileName)
    {
        return std::filesystem::path(fileName).remove_filename().string();
    }

    CS_API std::string ReadEntireFile(std::string fileName)
    {
        std::ifstream file(fileName);
        if(!file.is_open()) return "";
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    CS_API void WriteEntireFile(std::string fileName, std::string content)
    {
        std::ofstream file(fileName);
        if(!file.is_open()) return;
        file << content;
    }

    CS_API std::vector<u8> ReadBinaryFile(std::string fileName)
    {
        std::vector<u8> data;
        std::ifstream file(fileName, std::ios::binary);
        if(!file.is_open()) return data;
        data.resize(GetSizeOfFile(fileName));
        file.read(CS_CAST(char*, &data[0]), data.size()*sizeof(u8));
        return data;
    }

    CS_API void WriteBinaryFile(std::string fileName, void* data, size_t size)
    {
        std::ofstream file(fileName, std::ios::binary);
        if(!file.is_open()) return;
        file.write(CS_CAST(const char*, data), size);
    }

    CS_API void ListPathFiles(std::string pathName, std::vector<std::string>& files, bool recursive)
    {
        if(!DoesPathExist(pathName)) return;
        if(recursive)
            for(auto& p: std::filesystem::recursive_directory_iterator(pathName))
                if(IsFile(p.path().string())) files.push_back(p.path().string());
        else
            for(auto& p: std::filesystem::directory_iterator(pathName))
                if(IsFile(p.path().string())) files.push_back(p.path().string());
    }

    //
    // Input
    //

    CS_API const InputState& GetInput()
    {
        return s_context.input;
    }
}

int main(int argc, char** argv)
{
    CS_DEFER { CheckTrackedMemory(); };

    s_appConfig = csMain(argc, argv);
    CS_ASSERT(s_appConfig.app, "Need to define an application for the engine to run!");
    CS_DEFER { Deallocate(s_appConfig.app); };

    // Cache useful paths.
    s_context.execPath = ValidatePath(SDL_GetBasePath());
    s_context.dataPath = ValidatePath(SDL_GetPrefPath("ChromoStudio", s_appConfig.title.c_str()));

    CS_DEBUG_LOG("Starting Application %s...", s_appConfig.title.c_str());;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) CS_ERROR_LOG("Failed to initialize SDL2!");
    CS_DEFER { SDL_Quit(); };

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    s32 windowX = SDL_WINDOWPOS_CENTERED;
    s32 windowY = SDL_WINDOWPOS_CENTERED;
    s32 windowW = s_appConfig.window.size.x;
    s32 windowH = s_appConfig.window.size.y;
    bool windowVSync = s_appConfig.window.vSync;
    bool windowMaximized = s_appConfig.window.maximized;
    bool windowFullscreen = s_appConfig.window.fullscreen;
    s32 windowDisplay = 0;

    // @Incomplete: Reimplement window state loading!
    /*
    // If we have the previous session's window state stored then restore the window.
    json& engineState = GetState();
    if(engineState.contains("window"))
    {
        json windowState = engineState["window"];
        if(windowState.contains("bounds"))
        {
            windowX = GetJSONValueWithDefault<s32>(windowState["bounds"], "x", windowX);
            windowY = GetJSONValueWithDefault<s32>(windowState["bounds"], "y", windowY);
            windowW = GetJSONValueWithDefault<s32>(windowState["bounds"], "w", windowW);
            windowH = GetJSONValueWithDefault<s32>(windowState["bounds"], "h", windowH);
        }
        windowVSync = GetJSONValueWithDefault<bool>(windowState, "vsync", windowVSync);
        windowMaximized = GetJSONValueWithDefault<bool>(windowState, "maximized", windowMaximized);
        windowFullscreen = GetJSONValueWithDefault<bool>(windowState, "fullscreen", windowFullscreen);
        windowDisplay = GetJSONValueWithDefault<s32>(windowState, "display", windowDisplay);

        SDL_Rect displayBounds;
        if(SDL_GetDisplayBounds(windowDisplay, &displayBounds) < 0)
        {
            CS_DEBUG_LOG("Previous display no longer available, aborting window restore!");
        }
        else
        {
            // Make sure the window is not out of bounds at all.
            if(windowX != SDL_WINDOWPOS_CENTERED && windowX < displayBounds.x) windowX = displayBounds.x;
            if(windowX != SDL_WINDOWPOS_CENTERED && (windowX+windowW) >= displayBounds.w) windowX = displayBounds.w - windowW;
            if(windowY != SDL_WINDOWPOS_CENTERED && windowY < displayBounds.y) windowY = displayBounds.y;
            if(windowY != SDL_WINDOWPOS_CENTERED && (windowY+windowH) >= displayBounds.h) windowY = displayBounds.h - windowH;
        }
    }
    */

    s_context.window = SDL_CreateWindow(s_appConfig.title.c_str(), windowX,windowY,windowW,windowH, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
    if(!s_context.window) CS_ERROR_LOG("Failed to create window!");
    CS_DEFER { SDL_DestroyWindow(s_context.window); };
    SDL_SetWindowMinimumSize(s_context.window, s_appConfig.window.min.x, s_appConfig.window.min.y);

    s_context.glContext = SDL_GL_CreateContext(s_context.window);
    if(!s_context.glContext) CS_ERROR_LOG("Failed to create OpenGL context!");
    CS_DEFER { SDL_GL_DeleteContext(s_context.glContext); };

    glewInit();

    // Needs to happen after initializing the GL context as it's needed for VSync.
    EnableVSync(windowVSync);
    MaximizeWindow(windowMaximized);
    FullscreenWindow(windowFullscreen);

    InitAssetManager();
    CS_DEFER { QuitAssetManager(); };

    gfx::InitGraphics();
    CS_DEFER { gfx::QuitGraphics(); };

    sfx::InitAudio();
    CS_DEFER { sfx::QuitAudio(); };

    s_appConfig.app->OnInit();
    s_appConfig.app->m_running = true;

    // Frame timing variables.
    s64  desiredFrametime  = SDL_GetPerformanceFrequency() / CS_CAST(s32, s_appConfig.tickrate);
    s64  vSyncMaxError     = CS_CAST(s32, SDL_GetPerformanceFrequency() * 0.0002);
    s64  time60Hz          = SDL_GetPerformanceFrequency() / 60;
    s64  snapFrequencies[] = { time60Hz, time60Hz*2, time60Hz*3, time60Hz*4, (time60Hz+1)/2 };
    s64  timeAverager[]    = { desiredFrametime, desiredFrametime, desiredFrametime, desiredFrametime };
    s64  currFrameTime     = 0;
    s64  prevFrameTime     = SDL_GetPerformanceCounter();
    s64  deltaTime         = 0;
    s64  frameAccumulator  = 0;
    f32  fixedDeltaTime    = 1.0f / s_appConfig.tickrate;
    f32  consumedDeltaTime = 0.0f;
    bool resync            = true;

    while(s_appConfig.app->m_running)
    {
        currFrameTime = SDL_GetPerformanceCounter();
        deltaTime = currFrameTime - prevFrameTime;
        prevFrameTime = currFrameTime;

        if(deltaTime > desiredFrametime*8)
            deltaTime = desiredFrametime;
        if(deltaTime < 0)
            deltaTime = 0;

        for(s64 snap: snapFrequencies)
        {
            if(abs(deltaTime - snap) < vSyncMaxError)
            {
                deltaTime = snap;
                break;
            }
        }

        for(s32 i=0; i<CS_ARRAY_SIZE(timeAverager)-1; ++i)
            timeAverager[i] = timeAverager[i+1];
        timeAverager[CS_ARRAY_SIZE(timeAverager)-1] = deltaTime;
        deltaTime = 0;
        for(s32 i=0; i<CS_ARRAY_SIZE(timeAverager); ++i)
            deltaTime += timeAverager[i];
        deltaTime /= CS_ARRAY_SIZE(timeAverager);

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case(SDL_KEYDOWN):
                {
                    switch(event.key.keysym.sym)
                    {
                        case(SDLK_RETURN): if(!(SDL_GetModState()&KMOD_ALT)) break;
                        case(SDLK_F11): FullscreenWindow(!IsFullscreen()); break;
                    }
                } break;
                case(SDL_QUIT):
                {
                    s_appConfig.app->m_running = false;
                } break;
                case(SDL_WINDOWEVENT):
                {
                    if(event.window.windowID == SDL_GetWindowID(s_context.window))
                    {
                        switch(event.window.event)
                        {
                            case(SDL_WINDOWEVENT_MAXIMIZED): s_context.maximized = true; break;
                            case(SDL_WINDOWEVENT_RESTORED): s_context.maximized = false; break;
                        }
                    }
                } break;
                // Input events.
                case(SDL_CONTROLLERDEVICEADDED): AddGamepad(); break;
                case(SDL_CONTROLLERDEVICEREMOVED): RemoveGamepad(); break;
                case(SDL_MOUSEWHEEL):
                {
                    s_context.input.mouseWheel.x = event.wheel.x;
                    s_context.input.mouseWheel.y = event.wheel.y;
                } break;
            }
        }

        frameAccumulator += deltaTime;
        if(frameAccumulator > desiredFrametime*8)
            resync = true;

        if(resync)
        {
            frameAccumulator = 0;
            deltaTime = desiredFrametime;
            resync = false;
            for(s32 i=0; i<CS_ARRAY_SIZE(timeAverager); ++i)
                timeAverager[i] = desiredFrametime;
        }

        bool didUpdate = false;
        while(frameAccumulator >= desiredFrametime)
        {
            UpdateInputState();
            s_appConfig.app->OnUpdate(fixedDeltaTime);
            frameAccumulator -= desiredFrametime;
            didUpdate = true;
        }

        if(didUpdate)
        {
            gfx::SetViewport(NULL);
            gfx::Clear(s_appConfig.clearColor);
            gfx::BeginRenderFrame();
            s_appConfig.app->OnRender(fixedDeltaTime);
            if(IsDebugRender())
            {
                s_appConfig.app->OnDebugRender(fixedDeltaTime);
            }
        }
        gfx::EndRenderFrame();

        SDL_GL_SwapWindow(s_context.window);

        #if CS_DEBUG
        s_context.currentFPS = CS_CAST(f32, SDL_GetPerformanceFrequency()) / deltaTime;
        std::string title = s_appConfig.title + " (FPS: " + std::to_string(s_context.currentFPS) + ")";
        SDL_SetWindowTitle(s_context.window, title.c_str());
        #endif // CS_DEBUG

        // The window starts out hidden, after the first draw we unhide the window as this looks quite clean.
        if(CS_CHECK_FLAGS(SDL_GetWindowFlags(s_context.window), SDL_WINDOW_HIDDEN))
        {
            SDL_ShowWindow(s_context.window);
        }
    }

    s_appConfig.app->OnQuit();

    // Save the window state so it can be restored for future sessions.
    if(s_context.window)
    {
        SDL_RestoreWindow(s_context.window);
        if(!IsFullscreen())
        {
            SDL_GetWindowPosition(s_context.window, &windowX,&windowY);
            SDL_GetWindowSize(s_context.window, &windowW,&windowH);
        }
        else
        {
            // Use the cached bounds as they represent the actual window pos and size.
            windowX = s_context.cachedWindowXPos;
            windowY = s_context.cachedWindowYPos;
            windowW = s_context.cachedWindowWidth;
            windowH = s_context.cachedWindowHeight;
        }

        windowVSync = IsVSyncOn();
        windowMaximized = s_context.maximized;
        windowFullscreen = s_context.fullscreen;

        windowDisplay = SDL_GetWindowDisplayIndex(s_context.window);

        // @Incomplete: Reimplement window state saving!
        /*
        engineState["window"] = json::object();
        json& windowState = engineState["window"];

        windowState["bounds"]["x"] = windowX;
        windowState["bounds"]["y"] = windowY;
        windowState["bounds"]["w"] = windowW;
        windowState["bounds"]["h"] = windowH;
        windowState["vsync"] = windowVSync;
        windowState["maximized"] = windowMaximized;
        windowState["fullscreen"] = windowFullscreen;
        windowState["display"] = windowDisplay;
        */
    }

    return 0;
}
