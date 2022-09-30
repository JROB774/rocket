#include "cs_assets.hpp"
#include "cs_platform.hpp"

#include <algorithm>
#include <fstream>

using namespace cs;

CS_PUBLIC_SCOPE
{
    AssetManager s_assetManager;

    CS_API void InitAssetManager()
    {
        // The executable's location is the highest priority location for assets.
        s_assetManager.assetPaths.push_back(GetExecPath());

        // If there's other asset locations add them with descending priority.
        std::ifstream file("asset_paths.txt", std::ios::in);
        if(file.is_open())
        {
            std::string path;
            while(std::getline(file, path))
            {
                s_assetManager.assetPaths.push_back(GetExecPath() + ValidatePath(path));
            }
        }
    }

    CS_API void QuitAssetManager()
    {
        for(auto& [name,asset]: s_assetManager.assetMap)
        {
            if(asset)
            {
                asset->Free();
                Deallocate(asset);
            }
        }
        s_assetManager.assetMap.clear();
    }
}
