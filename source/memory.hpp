#define MEM_SYSTEM { __FILE__, __LINE__, "System", { 1,0,0,1 } }
#define MEM_GAME { __FILE__, __LINE__, "Game", { 1,1,0,1 } }
#define MEM_ASSET { __FILE__, __LINE__, "Asset", { 0,0,1,1 } }

struct MemoryTag
{
    const char* fileName;
    int         lineNumber;
    const char* category;
    nkVec4      color;
};

static void TrackMemory(const MemoryTag& tag, const char* typeName, size_t typeSize, size_t count, const void* data);
static void UntrackMemory(const void* data);

static void CheckTrackedMemory();

template<typename T>
static T* Allocate(const MemoryTag& tag, size_t count = 1);
template<typename T>
static void Deallocate(T* data);
