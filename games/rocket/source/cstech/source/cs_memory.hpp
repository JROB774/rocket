#pragma once

#include "cs_define.hpp"
#include "cs_math.hpp"

#include <typeinfo>

#define CS_MEM_SYSTEM { __FILE__, __LINE__, "System", cs::Vec4(1,0,0,1) }
#define CS_MEM_GAME { __FILE__, __LINE__, "Game", cs::Vec4(1,1,0,1) }
#define CS_MEM_ASSET { __FILE__, __LINE__, "Asset", cs::Vec4(0,0,1,1) }

CS_PUBLIC_SCOPE
{
    struct MemoryTag
    {
        const char* fileName;
        int lineNumber;
        const char* category;
        Vec4 color;
    };

    CS_API void TrackMemory(const MemoryTag& tag, const char* typeName, size_t typeSize, size_t count, const void* data);
    CS_API void UntrackMemory(const void* data);

    CS_API void CheckTrackedMemory();

    CS_API void MemoryTrackerDebugUi(bool& open);

    template<typename T>
    CS_API CS_INLINE T* Allocate(const MemoryTag& tag, size_t count = 1)
    {
        T* data = new T[count]; // Need to use new because some of are stuff uses STL and needs constructors...
        TrackMemory(tag, typeid(T).name(), sizeof(T), count, CS_CAST(void*, data));
        return data;
    }
    template<typename T>
    CS_API CS_INLINE void Deallocate(T* data)
    {
        UntrackMemory(CS_CAST(void*, data));
        delete[] data;
    }
}
