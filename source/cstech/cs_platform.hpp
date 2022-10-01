#pragma once

#include "cs_application.hpp"
#include "cs_input.hpp"

#include <string>
#include <vector>

CS_PUBLIC_SCOPE
{
    struct InputState
    {
        bool  hasGamepad;
        Vec2i mouseWheel;
        Vec2  mousePos;
        Vec2  relativeMousePos;
        bool  previousKeyState[KeyCode_TOTAL];
        bool  currentKeyState[KeyCode_TOTAL];
        bool  previousMouseButtonState[MouseButton_TOTAL];
        bool  currentMouseButtonState[MouseButton_TOTAL];
        bool  previousButtonState[GamepadButton_TOTAL];
        bool  currentButtonState[GamepadButton_TOTAL];
        s16   previousAxisState[GamepadAxis_TOTAL];
        s16   currentAxisState[GamepadAxis_TOTAL];
    };

    CS_API const AppConfig& GetAppConfig();

    CS_API f32 GetCurrentFPS();

    CS_API std::string GetExecPath();
    CS_API std::string GetDataPath();

    CS_API void ResetWindow();

    CS_API void PositionWindow(s32 x, s32 y);
    CS_API void ResizeWindow(s32 w, s32 h);

    CS_API void MaximizeWindow(bool enable);
    CS_API bool IsMaximized();
    CS_API void FullscreenWindow(bool enable);
    CS_API bool IsFullscreen();

    CS_API void EnableVSync(bool enable);
    CS_API bool IsVSyncOn();

    CS_API s32 GetWindowX();
    CS_API s32 GetWindowY();
    CS_API s32 GetWindowWidth();
    CS_API s32 GetWindowHeight();

    CS_API void ShowCursor(bool show);
    CS_API bool IsCursorVisible();

    CS_API void LockMouse(bool lock);
    CS_API bool IsMouseLocked();

    // File System
    CS_API bool            DoesFileExist(std::string fileName);
    CS_API bool            DoesPathExist(std::string pathName);
    CS_API bool            IsFile(std::string fileName);
    CS_API bool            IsPath(std::string pathName);
    CS_API std::string     ValidatePath(std::string pathName);
    CS_API bool            CreatePath(std::string pathName);
    CS_API size_t          GetSizeOfFile(std::string fileName);
    CS_API std::string     StripFileExtension(std::string fileName);
    CS_API std::string     StripFilePath(std::string fileName);
    CS_API std::string     StripFileExtensionAndPath(std::string fileName);
    CS_API std::string     StripFileName(std::string fileName);
    CS_API std::string     GetFileExtension(std::string fileName);
    CS_API std::string     GetFilePath(std::string fileName);
    CS_API std::string     ReadEntireFile(std::string fileName);
    CS_API void            WriteEntireFile(std::string fileName, std::string content);
    CS_API std::vector<u8> ReadBinaryFile(std::string fileName);
    CS_API void            WriteBinaryFile(std::string fileName, void* data, size_t size);
    CS_API void            ListPathFiles(std::string pathName, std::vector<std::string>& files, bool recursive);

    // Returns a structure with all the raw information about the input for this frame. It is best to not
    // use this function directly and instead use the wrapper functions in cs_input which provide a better
    // interface for querying specific input state information and conditions.
    CS_API const InputState& GetInput();
}
