#pragma once

#include "cs_define.hpp"
#include "cs_imgui.hpp"

#include <string>

#define CS_DEBUG_LOG(...) cs::DebugLog(__FILE__, __VA_ARGS__)
#define CS_ERROR_LOG(...) do { cs::DebugLog(__FILE__, __VA_ARGS__); abort(); } while(0)

CS_PUBLIC_SCOPE
{
    typedef void(*DebugUiWindowCallback)(bool& open);

    CS_API void InitDebugUi();
    CS_API void QuitDebugUi();

    CS_API void EnableDebugMode(bool enable);
    CS_API bool IsDebugMode();
    CS_API void EnableDebugRender(bool enable);
    CS_API bool IsDebugRender();

    CS_API void UpdateDebugUi();

    CS_API void BeginDebugUiFrame();
    CS_API void EndDebugUiFrame();

    CS_API void RegisterDebugUiWindow(std::string name, DebugUiWindowCallback windowCallback);
    CS_API void UnregisterDebugUiWindow(std::string name);

    CS_API bool DoesDebugUiWantMouseInput();

    CS_API Rect GetDebugGameViewport();

    // DO NOT CALL! Instead use the CS_DEBUG_LOG macro as it handles certain parameters.
    CS_API void DebugLog(const char* file, const char* format, ...);
}
