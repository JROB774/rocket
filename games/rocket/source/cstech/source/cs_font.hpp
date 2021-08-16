#pragma once

#include "cs_define.hpp"
#include "cs_assets.hpp"

CS_PUBLIC_SCOPE
{
    // @INCOMPLETE: Dummy asset so we can get font asset paths for ImGui, will implement later...
    struct Font
    {
        // Nothing...
    };

    CS_DECLARE_ASSET(Font)
    {
    public:
        Font m_data;

        bool        Load(std::string fileName) override;
        void        Free() override;
        void        DoDebugView() override;
        const char* GetPath() const override;
        const char* GetExt() const override;
        const char* GetType() const override;
    };
}
