#pragma once

#include "cs_memory.hpp"

#include <string>
#include <map>
#include <vector>

#define CS_DECLARE_ASSET(type) template<> class cs::Asset<type>: public cs::AssetBase

CS_PUBLIC_SCOPE
{
    // Base asset type, all assets should be specializations of Asset<T>.
    class AssetBase
    {
    public:
        virtual bool        Load(std::string fileName) = 0;
        virtual void        Free() = 0;
        virtual const char* GetPath() const = 0;
        virtual const char* GetExt() const = 0;
        virtual const char* GetType() const = 0;

        // For internal use.
        std::string m_name;
        std::string m_lookup;
        std::string m_fileName;
        bool        m_loaded = false;
    };
    template<typename T>
    class Asset: public AssetBase
    {
        // Nothing...
    };

    struct AssetManager
    {
        std::map<std::string,bool> assetFilters;
        std::map<std::string,AssetBase*> assetMap;
        std::vector<AssetBase*> assetList;
        std::vector<std::string> assetPaths;
    };

    extern AssetManager s_assetManager;

    CS_API void InitAssetManager();
    CS_API void QuitAssetManager();

    //
    // Asset Interface
    //

    template<typename T>
    CS_API std::string GetAssetPath(std::string name)
    {
        typedef Asset<T> AssetType;
        name = ValidatePath(name);

        AssetType dummy; // @Cleanup @Hack: Kind of hacky...

        std::string lookup = name + dummy.GetExt();
        std::string fileName;
        for(auto& assetPath: s_assetManager.assetPaths)
        {
            fileName = ValidatePath(assetPath + dummy.GetPath()) + lookup;
            if(DoesFileExist(fileName)) break;
        }
        return fileName;
    }

    template<typename T>
    CS_API bool LoadAsset(std::string name)
    {
        typedef Asset<T> AssetType;
        if(name.empty()) return false;
        name = ValidatePath(name);

        AssetType dummy; // @Cleanup @Hack: Kind of hacky...

        // Don't load the asset if it already exists.
        std::string lookup = name + dummy.GetExt();
        AssetType* asset = dynamic_cast<AssetType*>(s_assetManager.assetMap[lookup]);
        if(asset && asset->m_loaded) return true;
        // Load the asset is we need to.
        printf("Loading %s: %s\n", dummy.GetType(), name.c_str());
        if(!asset) asset = Allocate<AssetType>(CS_MEM_ASSET);
        if(!asset) return false;
        asset->m_name = ValidatePath(name);
        asset->m_lookup = lookup;
        asset->m_fileName = GetAssetPath<T>(asset->m_name);
        asset->m_loaded = asset->Load(asset->m_fileName);

        // Add it to the asset containers.
        s_assetManager.assetMap[lookup] = asset;
        auto& loc = std::find(s_assetManager.assetList.begin(), s_assetManager.assetList.end(), asset);
        if(loc != s_assetManager.assetList.end()) s_assetManager.assetList.erase(loc);
        s_assetManager.assetList.push_back(asset);

        // If the type is new add it to the debug UI filters.
        if(!Contains(s_assetManager.assetFilters, std::string(asset->GetType())))
            s_assetManager.assetFilters.insert({ asset->GetType(), true });

        return true;
    }

    template<typename T>
    CS_API T* GetAsset(std::string name)
    {
        typedef Asset<T> AssetType;
        if(name.empty()) return false;
        name = ValidatePath(name);

        AssetType dummy; // @Cleanup @Hack: Kind of hacky...

        // Get the asset if it is already loaded, otherwise load it now.
        std::string lookup = name + dummy.GetExt();
        AssetType* asset = dynamic_cast<AssetType*>(s_assetManager.assetMap[lookup]);
        if(!asset)
        {
            if(!LoadAsset<T>(name)) return NULL;
            asset = dynamic_cast<AssetType*>(s_assetManager.assetMap[lookup]);
            if(!asset) return NULL;
        }
        return ((asset->m_loaded) ? &asset->m_data : NULL);
    }

    template<typename T>
    CS_API void LoadAllAssetsOfType()
    {
        typedef Asset<T> AssetType;
        AssetType dummy; // @Cleanup @Hack: Kind of hacky...

        std::vector<std::string> files;
        for(auto& assetPath: s_assetManager.assetPaths)
        {
            std::string pathName = ValidatePath(assetPath + dummy.GetPath());
            ListPathFiles(pathName, files, true);
            for(auto& file: files) // Load all the valid asset files in the path.
            {
                if(GetFileExtension(file) == dummy.GetExt())
                {
                    std::string name = StripFileExtension(file);
                    name.erase(0, pathName.length());
                    LoadAsset<T>(name);
                }
            }
            files.clear();
        }
    }

    template<typename T>
    CS_API std::vector<T*> GetAllAssetsOfType()
    {
        typedef Asset<T> AssetType;
        AssetType dummy; // @Cleanup @Hack: Kind of hacky...

        std::vector<std::string> files;
        std::vector<T*> assets;

        for(auto& assetPath: s_assetManager.assetPaths)
        {
            std::string pathName = ValidatePath(assetPath + dummy.GetPath());
            ListPathFiles(pathName, files, true);
            for(auto& file: files) // Get all the valid asset files in the path.
            {
                if(GetFileExtension(file) == dummy.GetExt())
                {
                    std::string name = StripFileExtension(file);
                    name.erase(0, pathName.length());
                    T* asset = GetAsset<T>(name);
                    if(asset) assets.push_back(asset);
                }
            }
            files.clear();
        }

        return assets;
    }
}
