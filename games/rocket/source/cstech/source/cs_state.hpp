#pragma once

#include "cs_define.hpp"

#include <nlohmann/json.hpp>

CS_PUBLIC_SCOPE
{
    CS_API void LoadState();
    CS_API void SaveState();

    CS_API nlohmann::json& GetState();
}
