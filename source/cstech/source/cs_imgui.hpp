#pragma once

#include "cs_define.hpp"
#include "cs_math.hpp"

#define IMGUI_USER_CONFIG "cs_imgui.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

#define IM_VEC2_CLASS_EXTRA                                       \
ImVec2(const cs::Vec2& v) { x = v.x; y = v.y; }                   \
operator cs::Vec2() const { return cs::Vec2(x,y); }
#define IM_VEC4_CLASS_EXTRA                                       \
ImVec4(const cs::Vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; } \
operator cs::Vec4() const { return cs::Vec4(x,y,z,w); }

#define IM_ASSERT(e) CS_ASSERT(e, "ImGui Assert!")

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

// Convenient extensions to the ImGui API.
namespace ImGui
{
    IMGUI_API bool ToggleButton(const char* label, bool* value, const char* enabledLabel = NULL, const char* disabledLabel = NULL, const ImVec2& size = ImVec2(0,0));

    // Mark a section as diabled (cannot be interacted with).
    IMGUI_API void BeginDisabled();
    IMGUI_API void EndDisabled();
}
