#include "cs_imgui.hpp"

#include <string>

namespace ImGui
{
    IMGUI_API bool ToggleButton(const char* label, bool* value, const char* enabledLabel, const char* disabledLabel, const ImVec2& size)
    {
        IM_ASSERT(value);

        bool pressed = false;

        if(*value)
        {
            std::string labelText = label;
            if(enabledLabel)
                labelText = std::string(enabledLabel) + "###" + labelText;

            PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_CheckMark]);
            if(Button(labelText.c_str(), size))
            {
                *value = !*value;
                pressed = true;
            }
            PopStyleColor(1);
        }
        else
        {
            std::string labelText = label;
            if(disabledLabel)
                labelText = std::string(disabledLabel) + "###" + labelText;

            if(Button(labelText.c_str(), size))
            {
               *value = true;
               pressed = true;
            }
        }

        return pressed;
    }

    IMGUI_API void BeginDisabled()
    {
        PushItemFlag(ImGuiItemFlags_Disabled, true);
        PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
    }

    IMGUI_API void EndDisabled()
    {
        PopItemFlag();
        PopStyleVar();
    }
}
