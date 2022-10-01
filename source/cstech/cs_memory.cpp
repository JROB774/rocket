#include "cs_memory.hpp"
#include "cs_platform.hpp"

#include <map>

using namespace cs;

CS_PRIVATE_SCOPE
{
    struct MemoryMetaData
    {
        MemoryTag tag;
        const char* typeName;
        size_t typeSize;
        size_t count;
    };

    static std::map<const void*,MemoryMetaData> s_trackedMemory;
};

CS_PUBLIC_SCOPE
{
    CS_API void TrackMemory(const MemoryTag& tag, const char* typeName, size_t typeSize, size_t count, const void* data)
    {
        s_trackedMemory.insert({ data, { tag, typeName, typeSize, count } });
    }

    CS_API void UntrackMemory(const void* data)
    {
        s_trackedMemory.erase(data);
    }

    CS_API void CheckTrackedMemory()
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
}
