#include "cs_state.hpp"
#include "cs_utility.hpp"
#include "cs_debug.hpp"
#include "cs_platform.hpp"

using namespace nlohmann;

CS_PRIVATE_SCOPE
{
    static const char* k_stateFileName = "state.json";
    static json s_state;
}

CS_PUBLIC_SCOPE
{
    CS_API void LoadState()
    {
        CS_DEBUG_LOG("Looking for previous engine state...");
        std::string stateFileName = GetDataPath() + k_stateFileName;
        if(!DoesFileExist(stateFileName))
            CS_DEBUG_LOG("No previous engine state!");
        else
        {
            CS_DEBUG_LOG("Previous engine state found!");
            s_state = LoadJSONFromFile(stateFileName);
        }
    }

    CS_API void SaveState()
    {
        CS_DEBUG_LOG("Saving engine state...");
        std::string stateFileName = GetDataPath() + k_stateFileName;
        WriteEntireFile(stateFileName, s_state.dump(4));
    }

    CS_API json& GetState()
    {
        return s_state;
    }
}
