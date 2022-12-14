#define DECLARE_ASSET(type) template<> class Asset<type>: public AssetBase

// Base asset type, all assets should be specializations of Asset<T>.
class AssetBase
{
public:
    virtual bool        LoadFromFile(std::string fileName) = 0;
    virtual bool        LoadFromData(void* data, size_t bytes) = 0;
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
{;
    nkNPAK npak;
    bool   npakLoaded;

    std::map<std::string,bool>       assetFilters;
    std::map<std::string,AssetBase*> assetMap;
    std::vector<AssetBase*>          assetList;
    std::vector<std::string>         assetPaths;
};

static AssetManager s_assetManager;

static void InitAssetManager();
static void QuitAssetManager();

//
// Asset Interface
//

template<typename T>
static std::string GetAssetPath(std::string name);
template<typename T>
static bool LoadAsset(std::string name);
template<typename T>
static T* GetAsset(std::string name);
template<typename T>
static void LoadAllAssetsOfType();
template<typename T>
static std::vector<T*> GetAllAssetsOfType();
