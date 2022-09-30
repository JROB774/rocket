#pragma once

#include "cs_define.hpp"

#include <string>

#define CS_DEBUG_LOG(...) cs::DebugLog(__FILE__, __VA_ARGS__)
#define CS_ERROR_LOG(...) do { cs::DebugLog(__FILE__, __VA_ARGS__); abort(); } while(0)

CS_PUBLIC_SCOPE
{
    typedef void(*DebugUiWindowCallback)(bool& open);

    CS_API void EnableDebugRender(bool enable);
    CS_API bool IsDebugRender();

    // DO NOT CALL! Instead use the CS_DEBUG_LOG macro as it handles certain parameters.
    CS_API void DebugLog(const char* file, const char* format, ...);
}
