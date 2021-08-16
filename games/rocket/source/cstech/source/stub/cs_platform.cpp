#include "cs_platform.hpp"

using namespace cs;
using namespace gfx;

CS_PRIVATE_SCOPE
{
    // We don't actually use this but we need it so that GetAppConfig has something to return.
    static AppConfig s_dummyAppConfig;
}

CS_PUBLIC_SCOPE
{
    CS_API const AppConfig& GetAppConfig()
    {
        return s_dummyAppConfig;
    }

    CS_API f32 GetCurrentFPS()
    {
        return 0.0f;
    }

    CS_API std::string GetExecPath()
    {
        return "";
    }

    CS_API std::string GetDataPath()
    {
        return "";
    }

    CS_API void ResetWindow()
    {
        // Nothing...
    }

    CS_API void PositionWindow(s32 x, s32 y)
    {
        // Nothing...
    }

    CS_API void ResizeWindow(s32 w, s32 h)
    {
        // Nothing...
    }

    CS_API void MaximizeWindow(bool enable)
    {
        // Nothing...
    }

    CS_API bool IsMaximized()
    {
        return false;
    }

    CS_API void FullscreenWindow(bool enable)
    {
        // Nothing...
    }

    CS_API bool IsFullscreen()
    {
        return false;
    }

    CS_API void EnableVSync(bool enable)
    {
        // Nothing...
    }

    CS_API void IsVSyncOn()
    {
        return false;
    }

    CS_API s32 GetWindowX()
    {
        return 0;
    }

    CS_API s32 GetWindowY()
    {
        return 0;
    }

    CS_API s32 GetWindowWidth()
    {
        return 0;
    }

    CS_API s32 GetWindowHeight()
    {
        return 0;
    }

    CS_API void ShowCursor(bool show)
    {
        // Nothing...
    }

    CS_API bool IsCursorVisible()
    {
        return true;
    }

    CS_API void LockMouse(bool lock)
    {
        // Nothing...
    }

    CS_API bool IsMouseLocked()
    {
        return false;
    }

    CS_API bool DoesFileExist(std::string fileName)
    {
        return false;
    }

    CS_API bool DoesPathExist(std::string pathName)
    {
        return false;
    }

    CS_API bool IsFile(std::string fileName)
    {
        return false;
    }

    CS_API bool IsPath(std::string pathName)
    {
        return false;
    }

    CS_API std::string ValidatePath(std::string pathName)
    {
        return pathName;
    }

    CS_API bool CreatePath(std::string pathName)
    {
        return false;
    }

    CS_API size_t GetSizeOfFile(std::string fileName)
    {
        return 0;
    }

    CS_API std::string StripFileExtension(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string StripFilePath(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string StripFileExtensionAndPath(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string StripFileName(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string GetFileExtension(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string GetFilePath(std::string fileName)
    {
        return fileName;
    }

    CS_API std::string ReadEntireFile(std::string fileName)
    {
        return "";
    }

    CS_API void WriteEntireFile(std::string fileName, std::string content)
    {
        // Nothing...
    }

    CS_API std::vector<u8> ReadBinaryFile(std::string fileName)
    {
        return {};
    }

    CS_API void WriteBinaryFile(std::string fileName, void* data, size_t size)
    {
        // Nothing...
    }

    CS_API void ListPathFiles(std::string pathName, std::vector<std::string>& files, bool recursive)
    {
        // Nothing...
    }

    //
    // Keyboard
    //

    CS_API bool IsKeyDown(KeyCode code)
    {
        return false;
    }

    CS_API bool IsKeyUp(KeyCode code)
    {
        return false;
    }

    CS_API bool IsKeyPressed(KeyCode code)
    {
        return false;
    }

    CS_API bool IsKeyReleased(KeyCode code)
    {
        return false;
    }

    CS_API bool IsAnyKeyDown()
    {
        return false;
    }

    CS_API bool IsAnyKeyUp()
    {
        return false;
    }

    CS_API bool IsAnyKeyPressed()
    {
        return false;
    }

    CS_API bool IsAnyKeyReleased()
    {
        return false;
    }

    //
    // Mouse
    //

    CS_API s32 GetMouseScrollHorizontal()
    {
        return 0;
    }

    CS_API s32 GetMouseScrollVertical()
    {
        return 0;
    }

    CS_API Vec2 GetWindowMousePos()
    {
        return {};
    }

    CS_API Vec2 GetRelativeMousePos()
    {
        return {};
    }

    CS_API Vec2 GetScreenMousePos()
    {
        return {};
    }

    CS_API bool IsMouseButtonDown(MouseButton button)
    {
        return false;
    }

    CS_API bool IsMouseButtonUp(MouseButton button)
    {
        return false;
    }

    CS_API bool IsMouseButtonPressed(MouseButton button)
    {
        return false;
    }

    CS_API bool IsMouseButtonReleased(MouseButton button)
    {
        return fals;e
    }

    //
    // Gamepad
    //

    CS_API bool IsButtonDown(GamepadButton button)
    {
        return false;
    }

    CS_API bool IsButtonUp(GamepadButton button)
    {
        return false;
    }

    CS_API bool IsButtonPressed(GamepadButton button)
    {
        return false;
    }

    CS_API bool IsButtonReleased(GamepadButton button)
    {
        return false;
    }

    CS_API s16 GetAxis(GamepadAxis axis)
    {
        return 0;
    }

    CS_API bool IsRightStickUp()
    {
        return false;
    }

    CS_API bool IsRightStickRight()
    {
        return false;
    }

    CS_API bool IsRightStickDown()
    {
        return false;
    }

    CS_API bool IsRightStickLeft()
    {
        return false;
    }

    CS_API bool IsRightStickUpPressed()
    {
        return false;
    }

    CS_API bool IsRightStickRightPressed()
    {
        return false;
    }

    CS_API bool IsRightStickDownPressed()
    {
        return false;
    }

    CS_API bool IsRightStickLeftPressed()
    {
        return false;
    }

    CS_API bool IsLeftStickUp()
    {
        return false;
    }

    CS_API bool IsLeftStickRight()
    {
        return false;
    }

    CS_API bool IsLeftStickDown()
    {
        return false;
    }

    CS_API bool IsLeftStickLeft()
    {
        return false;
    }

    CS_API bool IsLeftStickUpPressed()
    {
        return false;
    }

    CS_API bool IsLeftStickRightPressed()
    {
        return false;
    }

    CS_API bool IsLeftStickDownPressed()
    {
        return false;
    }

    CS_API bool IsLeftStickLeftPressed()
    {
        return false;
    }
}

int main(int argc, char** argv)
{
    return 0;
}
