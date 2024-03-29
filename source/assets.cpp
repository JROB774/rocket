static void InitAssetManager()
{
    // Attempt to load the NPAK, if not then it doesn't matter.
    #if !defined(__EMSCRIPTEN__)
    std::string npakFilePath = GetExecPath() + "assets.npak";
    if(nk_npak_load(&s_assetManager.npak, npakFilePath.c_str()))
    {
        printf("Successfully loaded NPAK assets!\n");
        s_assetManager.npakLoaded = true;
    }
    #endif // __EMSCRIPTEN__

    // The executable's location is the highest priority location for assets.
    s_assetManager.assetPaths.push_back(GetExecPath());
    s_assetManager.assetPaths.push_back(GetExecPath() + "assets/");

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

static void QuitAssetManager()
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

    // We need to free the NPAK if we did end up loading it.
    #if !defined(__EMSCRIPTEN__)
    if(s_assetManager.npakLoaded)
    {
        nk_npak_free(&s_assetManager.npak);
    }
    #endif // __EMSCRIPTEN__
}

//
// Asset Interface
//

template<typename T>
static std::string GetAssetPath(std::string name)
{
    typedef Asset<T> AssetType;
    name = ValidatePath(name);

    AssetType dummy;

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
static bool LoadAsset(std::string name)
{
    typedef Asset<T> AssetType;
    if(name.empty()) return false;
    name = ValidatePath(name);

    AssetType dummy;

    // Don't load the asset if it already exists.
    std::string lookup = name + dummy.GetExt();
    AssetType* asset = dynamic_cast<AssetType*>(s_assetManager.assetMap[lookup]);
    if(asset && asset->m_loaded) return true;

    // Load the asset if we need to.
    printf("Loading %s: %s\n", dummy.GetType(), name.c_str());

    if(!asset) asset = Allocate<AssetType>(MEM_ASSET);
    if(!asset) return false;

    asset->m_name = ValidatePath(name);
    asset->m_lookup = lookup;
    asset->m_fileName = GetAssetPath<T>(asset->m_name);

    // First look in the NPAK and then fallback to looking on disk.
    #if !defined(__EMSCRIPTEN__)
    if(s_assetManager.npakLoaded)
    {
        std::string npakName = dummy.GetPath() + lookup;
        nkU64 fileSize;
        void* fileData = nk_npak_get_file_data(&s_assetManager.npak, npakName.c_str(), &fileSize);
        if(fileData)
        {
            asset->m_loaded = asset->LoadFromData(fileData, fileSize);
        }
    }
    #endif // __EMSCRIPTEN__
    if(!asset->m_loaded)
    {
        asset->m_loaded = asset->LoadFromFile(asset->m_fileName);
    }

    // Add it to the asset containers.
    s_assetManager.assetMap[lookup] = asset;
    auto loc = std::find(s_assetManager.assetList.begin(), s_assetManager.assetList.end(), asset);
    if(loc != s_assetManager.assetList.end()) s_assetManager.assetList.erase(loc);
    s_assetManager.assetList.push_back(asset);

    // If the type is new add it to the debug UI filters.
    if(!Contains(s_assetManager.assetFilters, std::string(asset->GetType())))
    {
        s_assetManager.assetFilters.insert({ asset->GetType(), true });
    }

    return true;
}

template<typename T>
static T* GetAsset(std::string name)
{
    typedef Asset<T> AssetType;
    if(name.empty()) return NULL;
    name = ValidatePath(name);

    AssetType dummy;

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
static void LoadAllAssetsOfType()
{
    typedef Asset<T> AssetType;
    AssetType dummy;

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
static std::vector<T*> GetAllAssetsOfType()
{
    typedef Asset<T> AssetType;
    AssetType dummy;

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
