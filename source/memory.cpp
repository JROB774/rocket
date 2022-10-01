struct MemoryMetaData
{
    MemoryTag tag;
    const char* typeName;
    size_t typeSize;
    size_t count;
};

static std::map<const void*,MemoryMetaData> s_trackedMemory;

static void TrackMemory(const MemoryTag& tag, const char* typeName, size_t typeSize, size_t count, const void* data)
{
    s_trackedMemory.insert({ data, { tag, typeName, typeSize, count } });
}

static void UntrackMemory(const void* data)
{
    s_trackedMemory.erase(data);
}

static void CheckTrackedMemory()
{
    if(!s_trackedMemory.empty())
    {
        printf("Memory Leaks Present:\n");
        for(auto [data,metaData]: s_trackedMemory)
        {
            std::string fileName = StripFileExtensionAndPath(metaData.tag.fileName);
            printf("[%s,%d] %s Memory: %zd %s (0x%p) %zd bytes\n", fileName.c_str(), metaData.tag.lineNumber,
                metaData.tag.category, metaData.count, metaData.typeName, data, metaData.count * metaData.typeSize);
        }
    }
}

template<typename T>
static T* Allocate(const MemoryTag& tag)
{
    T* data = new T; // Need to use new because some of our stuff uses STL and needs constructors...
    TrackMemory(tag, typeid(T).name(), sizeof(T), 1, CAST(void*, data));
    return data;
}

template<typename T>
static void Deallocate(T* data)
{
    UntrackMemory(CAST(void*, data));
    delete data;
}
