#include "cs_font.hpp"
#include "cs_debug.hpp"

CS_PUBLIC_SCOPE
{
    bool Asset<Font>::Load(std::string fileName)
    {
        // @INCOMPLETE: ...
        return true;
    }
    void Asset<Font>::Free()
    {
        // @INCOMPLETE: ...
    }
    void Asset<Font>::DoDebugView()
    {
        ImGui::Text("Font Asset");
    }
    const char* Asset<Font>::GetPath() const
    {
        return "fonts/";
    }
    const char* Asset<Font>::GetExt() const
    {
        return ".ttf";
    }
    const char* Asset<Font>::GetType() const
    {
        return "Font";
    }
}
